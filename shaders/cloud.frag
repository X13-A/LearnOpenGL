#version 330 core
out vec4 FragColor;
in vec3 fragPos;
in vec4 fragClipPos;

uniform vec3 boundsMin;
uniform vec3 boundsMax;
uniform vec3 cameraPos;
uniform sampler2D mainTex;
uniform sampler3D noiseTex;
uniform int numSteps;

float rand(vec3 seed)
{
    return fract(sin(dot(seed, vec3(12.9898, 78.233, 45.164))) * 43758.5453);
}

vec2 rayBoxDst(vec3 boundsMin, vec3 boundsMax, vec3 rayOrigin, vec3 rayDir)
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

void main()
{
	// WARNING: fragPos oscillates constantly when viewing angle changes.
	// TODO: Sampling the edges of the texture will cause flickering and a little bit of noise on the clouds. this should be fixed.
	
	//FragColor = texture(noiseTex, fragPos);
	//return;

    vec3 ndcPos = fragClipPos.xyz / fragClipPos.w;
	vec2 fragScreenPos = ndcPos.xy * 0.5 + 0.5;

	vec4 col = vec4(0, 0, 0, 0);
	vec3 rayOrigin = cameraPos;
	vec3 rayDir = normalize(fragPos - rayOrigin);
	
	vec2 rayBoxInfo = rayBoxDst(boundsMin, boundsMax, rayOrigin, rayDir);
	float dstToBox = rayBoxInfo.x;
	float dstInsideBox = rayBoxInfo.y;

	float dstTravelled = 0;
	float stepSize = dstInsideBox / numSteps;

	float totalDensity = 0;
	int i = 0;
	while (i < numSteps)
	{
		vec3 rayPos = rayOrigin + rayDir * (dstToBox + dstTravelled);
		float sampled_density = length(texture(noiseTex, rayPos/10).rgb);
		totalDensity += sampled_density * stepSize;
		dstTravelled += stepSize;
		i++;
	}

	float densityFactor = 0.01;
	if (dstInsideBox > 0)
	{
		vec4 cloudColor = vec4(1, 1, 1, 0) * min(0.75, totalDensity * densityFactor);
		vec4 res = texture(mainTex, fragScreenPos);
		res += cloudColor;
		FragColor = res;
	}
	else
	{
		FragColor = vec4(1, 0, 0, 1);
	}
}