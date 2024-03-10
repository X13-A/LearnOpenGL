#version 460 core

layout(local_size_x = 4, local_size_y = 4, local_size_z = 4) in;

layout(std430, binding = 0) buffer PointBuffer 
{
    float points[];
};

layout(binding = 1, rgba8) uniform image3D outputTexture;
uniform ivec3 textureDimensions;
uniform int pointsBufferLength;

void main() 
{
    ivec3 texelPos = ivec3(gl_GlobalInvocationID.xyz);
    vec3 normalizedPos = vec3(texelPos) / vec3(textureDimensions);

    float minDist = 10000000;
    for (int i = 0; i < pointsBufferLength;)
    {
        vec3 currentPoint = vec3(points[i++], points[i++], points[i++]);
        float newDist = distance(normalizedPos, currentPoint);
        if (newDist < minDist)
        {
            minDist = newDist;
        }
    }

    float worleyDiminutionFactor = 10;
    float worleyDist = (1 - minDist * worleyDiminutionFactor);
    vec4 color = vec4(vec3(worleyDist), 1);
    imageStore(outputTexture, texelPos, color);
}
