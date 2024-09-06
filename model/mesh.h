#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <vector>

#include "../shader/shader.h"
#include "vao.h"

enum TextureType { DIFFUSE, SPECULAR };

struct Texture {
    unsigned int id;
    TextureType type;
    std::string filename;
};

class Mesh {
    public:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
        void draw(Shader &shader);
        void drawInstanced(Shader &shader, std::vector<glm::mat4> modelMatrices);

    private:
        VertexArray vao;

        void setupMesh();

};

#endif
