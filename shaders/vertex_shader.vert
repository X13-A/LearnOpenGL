#version 330 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vColorIn;
layout (location = 2) in vec2 vTexCoordIn;

uniform float time;

out vec4 vColor;
out vec2 vTexCoord;

void main()
{
   float offset = 1; //cos(time*2);
   float intensity = (cos(time)+1.0f)/2.0f;
   gl_Position = vec4(vPos.x * offset, vPos.y * offset, vPos.z * offset, 1.0);

   // Output
   vColor = vec4(vColorIn, 1.0);
   vTexCoord = vTexCoordIn;
}