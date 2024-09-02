#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <vector>

#include "mesh.h"
#include "../stb_image/stb_image.h"

class Model {
    public:
        Model(std::string filename);
        void draw(Shader &shader);

        void drawInstanced(Shader &shader, std::vector<glm::mat4> modelMatrices);

    private:
        std::string directory;
        std::vector<Mesh> meshes;
        std::vector<Texture> loadedTextures;

        void loadModel(std::string filename);

        void processNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);

        std::vector<Texture> loadMaterial(aiMaterial *mat, aiTextureType type, TextureType typeName);
        unsigned int textureFromFile(std::string path);
        inline unsigned int loadTexture(std::string filename);
};

#endif
