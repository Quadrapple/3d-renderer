#include "model.h"

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

Model::Model(std::string filename) {
    loadModel(filename);
}

void Model::loadModel(std::string filename) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR! ASSIMP::" << importer.GetErrorString() << std::endl;
    }
    directory = filename.substr(0, filename.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene) {
    for(int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    for(int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    //Vertices
    if(mesh->HasTextureCoords(0)) {
        for(int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;

            vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            vertex.texCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);

            vertices.push_back(vertex);
        }
    } else {
        for(int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;

            vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            vertex.texCoords = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }
    }

    //Indices
    for(int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for(int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    //Textures and materials
    if(mesh->mMaterialIndex >= 0) {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<Texture> diffuse = loadMaterial(material, aiTextureType_DIFFUSE, DIFFUSE);
        textures.insert(textures.end(), diffuse.begin(), diffuse.end());

        std::vector<Texture> specular = loadMaterial(material, aiTextureType_SPECULAR, SPECULAR);
        textures.insert(textures.end(), specular.begin(), specular.end());
    }

    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterial(aiMaterial *material, aiTextureType type, TextureType typeName) {
    std::vector<Texture> res;
    for(int i = 0; i < material->GetTextureCount(type); i++) {
        aiString str;
        material->GetTexture(type, i, &str);
        
        Texture texture;
        std::string filename = directory + "/" + std::string(str.C_Str());
        texture.filename = filename;
        texture.type = typeName;

        texture.id = loadTexture(filename);

        loadedTextures.push_back(texture);
        res.push_back(texture);
    }
    return res;
}

inline unsigned int Model::loadTexture(std::string filename) {
    for(Texture texture: loadedTextures) {
        if(texture.filename == filename) {
            return texture.id;
        }
    }
    return textureFromFile(filename);
}

unsigned int Model::textureFromFile(std::string filename) {
    //Loading texture
    int width;
    int height;
    int nrChannels;

    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);

    //std::cout << "loaded texture from file " << filename << std::endl;

    if(!data) {
        std::cout << "ERROR! TEXTURE::LOAD_FAILED: " << std::endl  << filename << std::endl;
        stbi_image_free(data);
        return 0;
    }

    unsigned int textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    GLenum format;
    switch(nrChannels) {
        case 1:
            format = GL_RED;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, format /* convert into */, width, height, 0, format /* convert from */, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    glBindTexture(GL_TEXTURE_2D, 0);

    return textureId;
}
