#version 330 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec4 vColor;
layout (location = 2) in vec2 vTexCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec4 fragColor;
out vec2 fragTexCoord;

void main()
{
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vPos.xyz, 1.0);

	fragColor = vColor;
	fragTexCoord = vTexCoord;
}