#include "model_loader.h"
#include "texture_loader.h"

#include <format>
#include <iostream>


Model ModelLoader::load(std::string filename) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR! ASSIMP::" << importer.GetErrorString() << std::endl;
    }

    std::string directory = filename.substr(0, filename.find_last_of('/'));
    std::vector<Mesh> meshes;

    processNode(scene->mRootNode, scene, meshes, directory);

    return Model(meshes, directory);
}

void ModelLoader::processNode(aiNode *node, const aiScene *scene, std::vector<Mesh> &meshes, std::string &directory) {
    for(int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene, directory));
    }
    for(int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene, meshes, directory);
    }
}

Mesh ModelLoader::processMesh(aiMesh *mesh, const aiScene *scene, std::string &directory) {

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<std::shared_ptr<Texture>> textures;

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

        loadMaterial(material, aiTextureType_DIFFUSE, DIFFUSE, directory, textures);
        loadMaterial(material, aiTextureType_SPECULAR, SPECULAR, directory, textures);
    }

    return Mesh(vertices, indices, textures);
}

void ModelLoader::loadMaterial(aiMaterial *material, aiTextureType type, MapType mapType, std::string &directory, std::vector<std::shared_ptr<Texture>> &textures) {
    for(int i = 0; i < material->GetTextureCount(type); i++) {
        aiString filename;
        material->GetTexture(type, i, &filename);
        
        std::string path = std::format("{}/{}", directory, std::string(filename.C_Str()));

        const auto& texture = TextureLoader::load(path, GL_TEXTURE_2D);
        texture->mapType = mapType;

        textures.push_back(texture);
    }
}
