#include "texture_loader.h"

#include <format>
#include <iostream>

std::shared_ptr<Texture> TextureLoader::load(std::string path, GLenum type) {
    const auto& texPtr = loadedTextures.find(path);
    if(texPtr != loadedTextures.end() && !texPtr->second.expired()) {
        //std::cout << "Found already " << path << std::endl;
        return texPtr->second.lock(); //Avoids loading already loaded textures
    } else {
        //std::cout << "Loading new " << path << std::endl;
        auto res = std::make_shared<Texture>(loadFromFile(path, type));
        loadedTextures.emplace(path, std::weak_ptr<Texture>(res));
        return res;
    }
}

Texture TextureLoader::loadFromFile(std::string path, GLenum type) {
    //Loading texture
    int width;
    int height;
    int nrChannels;

    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    if(!data) {
        stbi_image_free(data);
        throw std::runtime_error(std::format("Error! Failed to load texture at {}", path));
    }

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

    Texture texture(type, format, {width, height}, format, data);

    stbi_image_free(data);

    return texture;
}
