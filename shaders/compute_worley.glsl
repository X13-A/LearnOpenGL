#version 430 core

layout(local_size_x = 4, local_size_y = 4, local_size_z = 4) in;

struct Point 
{
    vec3 position;
};

layout(std430, binding = 0) buffer PointBuffer 
{
    Point points[];
};

layout(binding = 1, rgba8) uniform image3D outputTexture;
uniform ivec3 textureDimensions;
uniform int pointsBufferLength;

void main() 
{
    ivec3 texelPos = ivec3(gl_GlobalInvocationID.xyz);

    float minDist = 10000000;
    for (int i = 0; i < pointsBufferLength; ++i)
    {
        float newDist = distance(vec3(texelPos) / vec3(textureDimensions), points[i].position);
        if (newDist < minDist)
        {
            minDist = newDist;
        }
    }
    float worleyDiminutionFactor = 5;
    float worleyIntensityFactor = 2;
    float worleyDist = (1 - minDist * worleyDiminutionFactor) * worleyIntensityFactor;
    
    vec4 color = vec4(worleyDist, worleyDist, worleyDist, 1);
    imageStore(outputTexture, texelPos, color);
}
