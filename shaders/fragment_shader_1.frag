#version 330 core
out vec4 FragColor;
uniform float time;

void main()
{
	float intensity = (cos(time)+1)/2;
	FragColor = vec4(0.5f, 0.5f, intensity, 1.0f);
}