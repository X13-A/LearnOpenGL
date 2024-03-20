#version 330 core
out vec4 FragColor;
in vec4 fragClipPos;
in vec3 fragPos;

uniform vec3 boundsMin;
uniform vec3 boundsMax;
uniform vec3 cameraDir;
uniform vec3 cameraPos;
uniform vec3 lightDir;
uniform vec3 lightPos;
uniform sampler2D mainTex;
uniform int numSteps;
uniform int numLightSteps;
uniform float global_brightness;
uniform float global_density;

uniform sampler3D noiseTex1;
uniform sampler3D noiseTex2;
uniform sampler3D noiseTex3;

uniform float global_scale1;
uniform float global_scale2;
uniform float global_scale3;

uniform vec3 global_speed1;
uniform vec3 global_speed2;
uniform vec3 global_speed3;

uniform vec3 phaseParams;

uniform float sunlightAbsorption;

uniform float time;
uniform float bottom_falloff;
uniform float top_falloff;

float sampleDensity(vec3 pos)
{
	vec3 speed1 = vec3(time * global_speed1);
	vec3 speed2 = vec3(time * global_speed2);
	vec3 speed3 = vec3(time * global_speed3);

	float density1 = length(texture(noiseTex1, (pos + speed1) / global_scale1).rgb);
	float density2 = length(texture(noiseTex2, (pos + speed2) / global_scale2).rgb);
	float density3 = length(texture(noiseTex3, (pos + speed3) / global_scale3).rgb);

	float res = min(density1, density2) - density3;

	//float top = max(boundsMin.y, boundsMax.y);
	//float bottom = min(boundsMin.y, boundsMax.y);
	//float y = pos.y;

	//float progress = clamp((y - bottom) / (top - bottom), 0, 1);
	return clamp(res, 0, 1);
}

// g = 0 causes uniform scattering while g = 1 causes directional scattering, in the direction of the photons
float henyeyGreenstein(float angle, float g)
{
	float g2 = g * g;
	return (1 - g2) / (4 * 3.1415 * pow(1 + g2 - 2 * g * (angle), 1.5));
}

float phase(vec3 rayDir, vec3 lightDir)
{
	// TODO: fix high luminance when far away
	float dot = dot(rayDir, lightDir);
	float hgTop = henyeyGreenstein(-dot, phaseParams.x);
	float hgBottom = henyeyGreenstein(dot, phaseParams.y);
	return phaseParams.z + hgBottom + 0;
}

vec2 rayBoxDst(vec3 rayOrigin, vec3 rayDir)
{
	vec3 invRayDir = 1 / rayDir;

	// Calculate ray intersections with box
	vec3 t0 = (boundsMin - rayOrigin) * invRayDir;
	vec3 t1 = (boundsMax - rayOrigin) * invRayDir;
	vec3 tmin = min(t0, t1);
	vec3 tmax = max(t0, t1);

	// Calculate distances
	float dstA = max(max(tmin.x, tmin.y), tmin.z); // A is the closest point
	float dstB = min(tmax.x, min(tmax.y, tmax.z)); // B is the furthest point

	float dstToBox = max(0, dstA);
	float dstInsideBox = max(0, dstB - dstToBox);
	return vec2(dstToBox, dstInsideBox);
}

float lightMarch(vec3 samplePos, vec3 lightDir)
{
	vec3 rayDir = -1 * lightDir;
	float dstTravelled = 0;

	// Evaluate distance inside box from samplePos to LightDir for computing stepSize
	float dstInsideBox = rayBoxDst(samplePos, rayDir).y;
	float stepSize = dstInsideBox / numLightSteps;
	float dstLimit = dstInsideBox;

	float totalDensity = 0;
	while (dstTravelled < dstLimit)
	{
		vec3 rayPos = samplePos + (rayDir * dstTravelled);
		float sampledDensity = stepSize * sampleDensity(rayPos) * global_density;
		totalDensity += sampledDensity;
		dstTravelled += stepSize;
	}

	return exp(-totalDensity * sunlightAbsorption);
}

vec2 sampleCloud(vec3 rayOrigin, vec3 rayDir, vec3 lightDir, float dstToBox, float dstInsideBox)
{
	float dstTravelled = 0;
	float dstLimit = min(100, dstInsideBox); // TODO: Improve far distances
	float baseStepSize = 0.5;

	float transmittance = 1;
	float lightEnergy = 0;
	float totalDensity = 0;
	float phaseVal = phase(rayDir, -lightDir);

	while (dstTravelled < dstLimit)
	{
		float stepSize = min(baseStepSize, dstLimit - dstTravelled); // TODO: Improve performance on far distances
		if (transmittance < 0.1)
		{
			break;
		}

		vec3 rayPos = rayOrigin + rayDir * (dstToBox + dstTravelled);
		float density = sampleDensity(rayPos) * stepSize * global_density;
		totalDensity += density;

		float lightTransmittance = lightMarch(rayPos, lightDir);
		lightEnergy += density * transmittance * lightTransmittance;
		transmittance *= exp(-density);

		dstTravelled += stepSize;
	}

	return vec2(transmittance, lightEnergy * phaseVal);
}

void main()
{
	vec3 ndcPos = fragClipPos.xyz / fragClipPos.w;
	vec2 fragScreenPos = ndcPos.xy * 0.5 + 0.5;

	vec3 rayDir = normalize(fragPos - cameraPos);
	vec3 dirToLight = normalize(lightPos - fragPos);

	// Calculate dist inside box	
	vec2 rayBoxInfo = rayBoxDst(cameraPos, rayDir);
	float dstToBox = rayBoxInfo.x;
	float dstInsideBox = rayBoxInfo.y;

	vec3 localLightDir = normalize(fragPos - lightPos);

	// Sample cloud density
	vec2 sampleData = sampleCloud(cameraPos, rayDir, localLightDir, dstToBox, dstInsideBox);
	float transmittance = sampleData.x;
	float lightEnergy = sampleData.y;

	// Sample base image
	vec4 backgroundColor = texture(mainTex, fragScreenPos);
	
	// Add atmosphere
	float skyLevel = max(0, dot(rayDir, vec3(0, 1, 0)));
	vec4 atmosphereColor = vec4(0);
	atmosphereColor += skyLevel * vec4(162.0 / 255.0, 213.0 / 255.0, 230.0 / 255.0, 1.0) * 1;
	atmosphereColor += (1 - skyLevel) * vec4(162.0 / 255.0, 213.0 / 255.0, 230.0 / 255.0, 1.0) * 1;
	backgroundColor += atmosphereColor;

	// Add sun
	float distToLight = distance(cameraPos, lightPos);
	float flareIntensity = dot(rayDir, dirToLight);
	flareIntensity = max(0, pow(flareIntensity, 512));
	vec4 sunColor = vec4(1) * flareIntensity;
	backgroundColor += vec4(vec3(flareIntensity), 0) * sunColor;

	// Add clouds
	if (dstInsideBox > 0)
	{
		vec4 cloudColor = vec4(1);
		cloudColor *= lightEnergy * global_brightness;

		FragColor = backgroundColor * transmittance + cloudColor;
		return;
	}

	FragColor = backgroundColor;
}