#version 330 core
out vec4 FragColor;

in vec4 fragColor;
in vec2 fragTexCoord;

uniform sampler2D m_Texture;

void main()
{
    // FragColor = texture(m_Texture, fragTexCoord);
    FragColor = fragColor;
}