#version 330 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vTexCoord;

uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;

out vec4 fragClipPos;
out vec3 fragPos;

void main()
{
    // DEBUG: world space quad
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vPos, 1.0);
    fragClipPos = gl_Position;
    fragPos = vec3(modelMatrix * vec4(vPos.xyz, 1.0));

    return;
}