#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

layout (std140) uniform Matrices {
    mat4 projection; // 64
    mat4 view; // 128
};

uniform mat4 models[128];

out vec2 TexCoord;
out vec3 Normal;

void main()
{
    //mat4 modelview = view * model;

    gl_Position = projection * view * models[gl_InstanceID] * vec4(aPos, 1.0f);
    TexCoord = aTexCoord;
    Normal = aNormal;
};
