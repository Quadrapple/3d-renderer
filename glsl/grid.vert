#version 330 core

layout (location = 0) in vec3 aPos;

layout (std140) uniform Matrices {
    mat4 projection; // 64
    mat4 view; // 128
};

void main()
{
    gl_Position = projection * view * vec4(aPos, 1.0f);
};

