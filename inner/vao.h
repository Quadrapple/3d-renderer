#ifndef VAO_H
#define VAO_H

#include "buffer.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

struct VertexAttrib {
    unsigned int count;
    unsigned int stride;
    GLenum type;
    const void *ptr;
};

class VertexArray {
    public:
        VertexArray();
        VertexArray(Buffer &vertices); //Sets default attributes

        void bind();

        void setBuffer(Buffer &buf);

        void enableAttrib(unsigned int index);
        void disableAttrib(unsigned int index);

        unsigned int getId() const;
        void setDefaultAttribs();
        void addAttrib(const VertexAttrib &attrib);

    private:
        unsigned int attribCount;
        unsigned int id;

};

#endif
