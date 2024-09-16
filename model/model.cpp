#include "model.h"

#include "model_loader.h"

void Model::draw(Shader &shader) {

    for(int i = 0; i < meshes.size(); i++) {
        meshes[i].draw(shader);
    }
}

void Model::drawInstanced(Shader &shader, std::vector<glm::mat4> modelMatrices) {
    for(int i = 0; i < meshes.size(); i++) {
        meshes[i].drawInstanced(shader, modelMatrices);
    }
}

std::string Model::getDirectory() const {
    return directory;
}

Model::Model(std::string path) {
    *this = ModelLoader::load(path);
}

Model::Model(std::vector<Mesh> meshes, std::string directory) {
    this->meshes = meshes;
    this->directory = directory;
}
