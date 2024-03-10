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
uniform sampler3D noiseTex;
uniform int numSteps;
uniform int numLightSteps;
uniform float global_brightness;
uniform float global_density;
uniform float global_scale;
uniform float sunlightAbsorption;

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
		float sampledDensity = stepSize * length(texture(noiseTex, rayPos / global_scale).rgb) * global_density;
		totalDensity += sampledDensity;
		dstTravelled += stepSize;
		i++;
	}

	float transmittance = exp(-totalDensity * sunlightAbsorption);
	return transmittance;
}

vec2 sampleCloud(vec3 rayOrigin, vec3 rayDir, float dstToBox, float dstInsideBox)
{
	float dstLimit = dstInsideBox;
	float dstTravelled = 0;

	float stepSize = 1; //TODO: replace with fixed value (and max range)

	float transmittance = 1;
	float lightEnergy = 0;

	int i = 0;
	while (dstTravelled < dstLimit)
	{
		vec3 rayPos = rayOrigin + rayDir * (dstToBox + dstTravelled);
		float density = length(texture(noiseTex, rayPos / global_scale).rgb) * stepSize * global_density;

		float lightTransmittance = lightMarch(rayPos);
		lightEnergy += density * transmittance * lightTransmittance; // TODO: Understand this line
		transmittance *= exp(-density);

		dstTravelled += stepSize;
		i++;
	}

	return vec2(transmittance, lightEnergy);
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
	vec2 sampleData = sampleCloud(rayOrigin, rayDir, dstToBox, dstInsideBox);
	float transmittance = sampleData.x;
	float lightEnergy = sampleData.y;

	if (dstInsideBox > 0)
	{
		// Sample base image
		vec3 ndcPos = fragClipPos.xyz / fragClipPos.w;
		vec2 fragScreenPos = ndcPos.xy * 0.5 + 0.5;
		vec4 backgroundColor = texture(mainTex, fragScreenPos);

		vec4 cloudColor = vec4(1, 1, 1, 1);
		cloudColor *= lightEnergy * global_brightness;

		FragColor = backgroundColor * transmittance + cloudColor;
		return;
	}
	FragColor = vec4(1, 0, 0, 1);
}