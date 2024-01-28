#version 330 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec4 vColor;
layout (location = 2) in vec2 vTexCoord;
layout (location = 3) in vec3 vNormal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec4 fragColor;
out vec2 fragTexCoord;
out vec3 fragPos;
out vec3 fragNormal;

void main()
{
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vPos, 1.0);

	// Output
	fragColor = vColor;
	fragPos = vec3(modelMatrix * vec4(vPos.xyz, 1.0));
	fragNormal = mat3(transpose(inverse(modelMatrix))) * vNormal;  
	fragTexCoord = vTexCoord;
}