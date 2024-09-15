
#include "mesh.h"
#include <iostream>
#include "../inner/vao.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<std::shared_ptr<Texture>> textures) {
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    setupMesh();
}

void Mesh::setupMesh() {
    vao.bind();
    Buffer vbo(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data());
    Buffer ebo(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data());
    
    vao.setBuffer(vbo);
    vao.setBuffer(ebo);
    vao.setDefaultAttribs();
}

void Mesh::drawInstanced(Shader &shader, std::vector<glm::mat4> modelMatrices) {
    unsigned int diffuseNr = 0;
    unsigned int specularNr = 0;
    shader.use();

    for(int i = 0; i < textures.size(); i++) {
        std::string name;
        switch (textures[i]->mapType) {
            case DIFFUSE:
                name = "diffuse_map" + std::to_string(diffuseNr);
                diffuseNr++;
                break;
            case SPECULAR:
                name = "specular_map" + std::to_string(specularNr);
                specularNr++;
                break;
        }

        shader.setInt("material." + name, i);
        textures[i]->bind(i);
    }

    //glActiveTexture(GL_TEXTURE0);
    for(int i = 0; i < modelMatrices.size(); i++) {
        shader.setMat4("models[" + std::to_string(i) + "]", modelMatrices[i]);
    }

    vao.bind();
    glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, modelMatrices.size());
}

void Mesh::draw(Shader &shader) {
    unsigned int diffuseNr = 0;
    unsigned int specularNr = 0;
    shader.use();

    for(int i = 0; i < textures.size(); i++) {
        textures[i]->bind(i);

        std::string name;
        switch (textures[i]->mapType) {
            case DIFFUSE:
                name = "diffuse_map" + std::to_string(diffuseNr);
                diffuseNr++;
                break;
            case SPECULAR:
                name = "specular_map" + std::to_string(specularNr);
                specularNr++;
                break;
        }

        shader.setInt("material." + name, i);
    }
    //glActiveTexture(GL_TEXTURE0);

    vao.bind();
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
