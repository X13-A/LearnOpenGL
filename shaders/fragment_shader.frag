#version 330 core
out vec4 FragColor;
in vec4 vColor;
in vec2 vTexCoord;
uniform sampler2D m_Texture1;
uniform sampler2D m_Texture2;

void main()
{
    FragColor = mix(texture(m_Texture1, vTexCoord), texture(m_Texture2, vTexCoord), 0.8);
}