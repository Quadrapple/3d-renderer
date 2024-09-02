#include "mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) {
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    setupMesh();
}

void Mesh::setupMesh() {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    unsigned int ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    
    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)offsetof(Vertex, normal)); 
    glEnableVertexAttribArray(1);

    // Texture coordinate
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
}

void Mesh::drawInstanced(Shader &shader, std::vector<glm::mat4> modelMatrices) {
    unsigned int diffuseNr = 0;
    unsigned int specularNr = 0;
    shader.use();

    for(int i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);

        std::string name;
        switch (textures[i].type) {
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
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    glActiveTexture(GL_TEXTURE0);
    for(int i = 0; i < modelMatrices.size(); i++) {
        shader.setMat4("models[" + std::to_string(i) + "]", modelMatrices[i]);
    }

    glBindVertexArray(vao);
    glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, modelMatrices.size());
    glBindVertexArray(0);
}

void Mesh::draw(Shader &shader) {
    unsigned int diffuseNr = 0;
    unsigned int specularNr = 0;
    shader.use();

    for(int i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);

        std::string name;
        switch (textures[i].type) {
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
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
