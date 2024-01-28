#version 330 core
layout (location = 0) in vec3 vPos;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec4 fragClipPos;
out vec3 fragPos;

void main()
{
	// Transform vertex position to clip space
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vPos.xyz, 1.0);
    fragClipPos = gl_Position;

    // Output vertex position in world space
    fragPos = vec3(modelMatrix * vec4(vPos.xyz, 1.0));
}