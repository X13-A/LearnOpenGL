#version 330 core
layout (location = 0) in vec4 vPos;
layout (location = 1) in vec4 vColorIn;
layout (location = 2) in vec2 vTexCoordIn;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec4 vColor;
out vec2 vTexCoord;

void main()
{
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vPos.xyz, 1.0);

	// Output
	vColor = vColorIn;
	vTexCoord = vTexCoordIn;
}