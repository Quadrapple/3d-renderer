#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>

enum MapType { DIFFUSE, SPECULAR };


class Texture {
    public:
        Texture();
        Texture(std::string path);

        Texture(GLenum type, GLenum internalFormat, glm::uvec2 size, GLenum origFormat, const unsigned char *data);

        //Multisampled
        Texture(GLenum type, GLenum internalFormat, glm::uvec2 size, unsigned int samples, bool fixedSamplePosition);

        unsigned int getId();
        std::string getPath();
        GLenum getType();
        MapType mapType;

        void bind(unsigned int unit);
        void texParameter(GLenum param, GLenum value);

        void texImage2D(GLenum internalFormat, GLenum origFormat, const unsigned char *data);
        void texImage2DMult(GLenum internalFormat, unsigned int samples, bool fixedSampleLocations);

    private:
        unsigned int id;
        glm::uvec2 size;
        GLenum type;
        std::string path;
};

#endif

