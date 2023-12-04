#version 330 core
layout (location = 0) in vec3 vPos;
uniform float time;

void main()
{
   float offset = cos(time*2);
   gl_Position = vec4(vPos.x * offset, vPos.y * offset, vPos.z * offset, 1.0);
}