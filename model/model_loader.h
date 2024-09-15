#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include "model.h"

class ModelLoader {
    public:
        static Model load(std::string path);

    private:
        static void processNode(aiNode *node, const aiScene *scene, std::vector<Mesh> &meshes, std::string &directory);
        static Mesh processMesh(aiMesh *mesh, const aiScene *scene, std::string &directory);

        static void loadMaterial(aiMaterial *mat, aiTextureType type, MapType mapType, std::string &directory, std::vector<std::shared_ptr<Texture>> &textures);
};

#endif
