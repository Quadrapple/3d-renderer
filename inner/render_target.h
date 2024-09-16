#ifndef RENDER_TARGET_H
#define RENDER_TARGET_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <vector>

#include "texture.h"

struct Renderbuffer {
    unsigned int id;
    GLenum type;
    glm::uvec2 size;
    unsigned int samples = 1;

    Renderbuffer(GLenum type, glm::uvec2 size);
    Renderbuffer(GLenum type, glm::uvec2 size, unsigned int samples);
    Renderbuffer();
};

class RenderTarget {
    public:
        RenderTarget();
        RenderTarget(glm::uvec2 size);
        RenderTarget(const std::shared_ptr<Texture> color);

        RenderTarget(glm::uvec2 size, unsigned int samples);

        void bind() const;
        void assertComplete() const;
        void blit(const RenderTarget &other) const;

        Texture& getColor(unsigned int attachment) const;
        unsigned int getId() const;

        RenderTarget& operator<<(const std::shared_ptr<Texture> rhs);
        RenderTarget& operator<<(const Renderbuffer &rhs);
    private:
        unsigned int id;
        glm::uvec2 size;

        std::vector<std::shared_ptr<Texture>> colorAttachments;
        Renderbuffer rbo;

};


#endif

