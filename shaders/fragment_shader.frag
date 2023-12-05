#version 330 core
out vec4 FragColor;
in vec4 vColor;
in vec2 vTexCoord;
uniform sampler2D m_Texture;

void main()
{
	FragColor = texture(m_Texture, vTexCoord) * vec4(vColor);
}