#version 330 core
layout (location = 0) in vec4 vPos;
layout (location = 1) in vec4 vColorIn;
layout (location = 2) in vec2 vTexCoordIn;

uniform float time;
uniform mat4 transform;
out vec4 vColor;
out vec2 vTexCoord;

void main()
{
		
	gl_Position = transform * vPos;

	// Output
	vColor = vColorIn;
	vTexCoord = vTexCoordIn;
}