#version 330 core
layout (location = 0) in vec3 vPos;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 fragPos;

void main()
{
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vPos.xyz, 1.0);

	// Output
	fragPos = vec3(modelMatrix * vec4(vPos.xyz, 1.0));
}