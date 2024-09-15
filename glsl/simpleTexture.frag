#version 330 core

in vec2 TexCoord;
in vec3 Normal;

struct Material {
    sampler2D diffuse_map0; 
};


uniform Material material;

out vec4 FragColor;

void main() {
    vec3 color = texture(material.diffuse_map0, TexCoord).xyz * clamp(dot(Normal, vec3(0.0, 1.0, 0.0)), 0.15, 1.0);
    FragColor = vec4(color, 1.0);
}
