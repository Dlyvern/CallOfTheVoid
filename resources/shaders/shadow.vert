#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 5) in ivec4 boneIds;
layout (location = 6) in vec4 weights;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

void main()
{
    mat4 boneTransform = mat4(0.0);
    for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
        if (boneIds[i] >= 0)
            boneTransform += finalBonesMatrices[boneIds[i]] * weights[i];
    }
    if (boneTransform == mat4(0.0))
        boneTransform = mat4(1.0);

    gl_Position = lightSpaceMatrix * model * boneTransform * vec4(pos, 1.0);
}