#version 330 core
out vec4 FragColor;
in vec3 fragPos;
in vec4 fragClipPos;

uniform vec3 boundsMin;
uniform vec3 boundsMax;
uniform vec3 cameraPos;
uniform vec3 cameraDir;
uniform vec3 lightDir;
uniform sampler2D mainTex;
uniform int numSteps;
uniform int numLightSteps;
uniform float global_brightness;
uniform float global_density;
uniform sampler3D noiseTex1;
uniform sampler3D noiseTex2;
uniform float global_scale1;
uniform float global_scale2;
uniform float sunlightAbsorption;

uniform float bottom_falloff;
uniform float top_falloff;

float sampleDensity(vec3 pos)
{
	float density1 = length(texture(noiseTex1, pos / global_scale1).rgb);
	float density2 = length(texture(noiseTex2, pos / global_scale2).rgb);

	return min(density1, density2);
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

float lightMarch(vec3 samplePos)
{
	vec3 rayDir = -1 * lightDir;
	float dstTravelled = 0;

	// Evaluate distance inside box from samplePos to LightDir for computing stepSize
	float dstInsideBox = rayBoxDst(samplePos, rayDir).y;
	float stepSize = dstInsideBox / numLightSteps;

	float totalDensity = 0;
	int i = 0;
	while (i < numLightSteps)
	{
		vec3 rayPos = samplePos + (rayDir * dstTravelled);
		float sampledDensity = stepSize * sampleDensity(rayPos) * global_density;
		totalDensity += sampledDensity;
		dstTravelled += stepSize;
		i++;
	}

	float transmittance = exp(-totalDensity * sunlightAbsorption);
	return transmittance;
}

vec3 sampleCloud(vec3 rayOrigin, vec3 rayDir, float dstToBox, float dstInsideBox)
{
	float dstTravelled = 0;
	float dstLimit = min(100, dstInsideBox); // TODO: Improve far distances
	float baseStepSize = 1;

	float transmittance = 1;
	float lightEnergy = 0;
	float stepSize = 0.5; // TODO: Improve performance on far distances
	float totalDensity = 0;

	int i = 0;
	while (dstTravelled < dstLimit)
	{
		if (transmittance < 0.02)
		{
			break;
		}
		//float stepSize = min(baseStepSize, dstInsideBox - dstTravelled);

		vec3 rayPos = rayOrigin + rayDir * (dstToBox + dstTravelled);
		float density = sampleDensity(rayPos) * stepSize * global_density;
		totalDensity += density;

		float lightTransmittance = lightMarch(rayPos);
		lightEnergy += density * transmittance * lightTransmittance;
		transmittance *= exp(-density);

		dstTravelled += stepSize;
		i++;
	}

	return vec3(transmittance, lightEnergy, totalDensity);
}

void main()
{
	vec3 rayOrigin = cameraPos;
	vec3 rayDir = normalize(fragPos - rayOrigin);

	// Calculate dist inside box	
	vec2 rayBoxInfo = rayBoxDst(rayOrigin, rayDir);
	float dstToBox = rayBoxInfo.x;
	float dstInsideBox = rayBoxInfo.y;

	// Sample cloud density
	vec3 sampleData = sampleCloud(rayOrigin, rayDir, dstToBox, dstInsideBox);
	float transmittance = sampleData.x;
	float lightEnergy = sampleData.y;
	float totalDensity = sampleData.z;

	if (dstInsideBox > 0)
	{
		// Sample base image
		vec3 ndcPos = fragClipPos.xyz / fragClipPos.w;
		vec2 fragScreenPos = ndcPos.xy * 0.5 + 0.5;
		vec4 backgroundColor = texture(mainTex, fragScreenPos);

		// Phase
		float cosAngle = dot(rayDir, -lightDir);
		float phaseVal = 1;

		vec4 cloudColor = vec4(1, 1, 1, 1);
		cloudColor *= lightEnergy * global_brightness * phaseVal;

		FragColor = backgroundColor* transmittance + cloudColor;
		return;
	}
	FragColor = vec4(1, 0, 0, 1);
}