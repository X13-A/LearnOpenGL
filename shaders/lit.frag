#version 330 core
out vec4 FragColor;

in vec4 fragColor;
in vec4 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoord;

uniform sampler2D m_Texture;
uniform vec4 lightColor;
uniform vec4 ambientColor;
uniform vec3 lightPos;

void main()
{
    //FragColor = texture(m_Texture, fragTexCoord) * (ambientColor + fragColor * lightColor);
    //float distX = distance(fragPos.x, lightPos.x);
    //float distY = distance(fragPos.y, lightPos.y);
    //float distZ = distance(fragPos.z, lightPos.z);
    //FragColor = vec4(distX, distY, distZ, 1.0);
    //FragColor = vec4(fragNormal.x, fragNormal.y, fragNormal.z, 1.0);

    vec4 lightDir = normalize(vec4(lightPos, 1.0) - fragPos);
    vec3 normal = normalize(fragNormal);

    float diffuseIntensity = max(dot(normal, vec3(lightDir)), 0.0);
    vec4 diffuseColor = lightColor * diffuseIntensity;

    vec4 lightColor = (ambientColor + diffuseColor) * fragColor;
    FragColor = texture(m_Texture, fragTexCoord) * lightColor;
}