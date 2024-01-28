#version 330 core
out vec4 FragColor;
in vec3 fragPos;
in vec4 fragClipPos;

uniform vec3 boundsMin;
uniform vec3 boundsMax;
uniform vec3 cameraPos;
uniform sampler2D mainTex;

vec2 rayBoxDst(vec3 boundsMin, vec3 boundsMax, vec3 rayOrigin, vec3 rayDir)
{
	vec3 t0 = (boundsMin - rayOrigin) / rayDir;
	vec3 t1 = (boundsMax - rayOrigin) / rayDir;
	vec3 tmin = min(t0, t1);
    vec3 tmax = max(t0, t1);

	float dstA = max(max(tmin.x, tmin.y), tmin.z);
    float dstB = min(tmax.x, min(tmax.y, tmax.z));

	float dstToBox = max(0, dstA);
    float dstInsideBox = max(0, dstB - dstToBox);
    return vec2(dstToBox, dstInsideBox);
}

void main()
{
    vec3 ndcPos = fragClipPos.xyz / fragClipPos.w;
	vec2 fragScreenPos = ndcPos.xy * 0.5 + 0.5;
	
	vec4 col = vec4(0, 0, 0, 0);
	vec3 rayOrigin = cameraPos;
	vec3 rayDir = normalize(fragPos - rayOrigin);
	
	vec2 rayBoxInfo = rayBoxDst(boundsMin, boundsMax, rayOrigin, rayDir);
	float dstToBox = rayBoxInfo.x;
	float dstInsideBox = rayBoxInfo.y / 3;
	
	if (dstInsideBox > 0)
	{
		vec4 res = texture(mainTex, fragScreenPos);
		res += vec4(dstInsideBox, dstInsideBox, dstInsideBox, 0);
		FragColor = res;
	}
	else
	{
		FragColor = vec4(1, 0, 0, 1);
	}
}