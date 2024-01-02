#version 330 core
out vec4 FragColor;

in vec4 fragColor;
in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoord;

uniform sampler2D m_Texture;
uniform vec4 lightColor;
uniform vec4 ambientColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    vec3 lightDir = normalize(lightPos - fragPos);
    vec3 normal = normalize(fragNormal);

    // Diffuse
    float diffuseIntensity = max(dot(normal, lightDir), 0.0);
    vec4 diffuseColor = lightColor * diffuseIntensity;

    // Specular
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectionDir = reflect(-lightDir, normal);
    float specularStrength = 0.5;
    float specularIntensity = pow(max(dot(viewDir, reflectionDir), 0.0), 128);
    vec4 specularColor = specularStrength * specularIntensity * lightColor;

    // Mix
    vec4 textureColor = texture(m_Texture, fragTexCoord);
    vec4 lightColor = (ambientColor + diffuseColor + specularColor);
    FragColor = fragColor * lightColor;
}