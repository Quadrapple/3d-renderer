#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H

#include "../stb_image/stb_image.h"
#include "mesh.h"
#include <unordered_map>
#include <memory>

class TextureLoader {
    public:
        static std::shared_ptr<Texture> load(std::string path, GLenum type);

    private:
        static inline std::unordered_map<std::string, std::weak_ptr<Texture>> loadedTextures; //Maps paths to textures
        static std::shared_ptr<Texture> loadFromFile(std::string path, GLenum type);

};

#endif

