#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 tex;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
layout(location = 5) in ivec4 boneIds;
layout(location = 6) in vec4 weights;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;

uniform mat4 finalBonesMatrices[MAX_BONES];
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vs_out;


void main()
{
    mat4 boneTransform = mat4(0.0);
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        if (boneIds[i] >= 0)
        {
            boneTransform += finalBonesMatrices[boneIds[i]] * weights[i];
        }
    }

    if (boneTransform == mat4(0.0))
    boneTransform = mat4(1.0);

    vec4 worldPos = model * boneTransform * vec4(pos, 1.0);
    vs_out.FragPos = vec3(worldPos);
    vs_out.Normal = mat3(transpose(inverse(model * boneTransform))) * norm;
    vs_out.TexCoords = tex;
    vs_out.FragPosLightSpace = lightSpaceMatrix * worldPos;

    gl_Position = projection * view * worldPos;
}
