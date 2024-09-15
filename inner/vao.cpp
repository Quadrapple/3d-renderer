#include "vao.h"
#include "buffer.h"
#include "state.h"

VertexArray::VertexArray() : attribCount(0){
    glGenVertexArrays(1, &id);
}

VertexArray::VertexArray(Buffer& vertices) {
    glGenVertexArrays(1, &id);
    bind();
    vertices.bind();
    setDefaultAttribs();
}

void VertexArray::bind() {
    State::getContext().bindVertexArray(id);
}

void VertexArray::addAttrib(const VertexAttrib &attrib) {
    bind();
    glVertexAttribPointer(attribCount, attrib.count, attrib.type, GL_FALSE, attrib.stride, attrib.ptr);
    glEnableVertexAttribArray(attribCount);
    attribCount++;
}

void VertexArray::setBuffer(Buffer &buf) {
    bind();
    buf.bind();
}

void VertexArray::setDefaultAttribs() {
    bind();
    attribCount = 3;
    //Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    //Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)offsetof(Vertex, normal)); 
    glEnableVertexAttribArray(1);

    //TexCoord
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(2);
}

void VertexArray::enableAttrib(unsigned int index) {
    bind();
    glEnableVertexAttribArray(index);
}

void VertexArray::disableAttrib(unsigned int index) {
    bind();
    glEnableVertexAttribArray(index);
}

unsigned int VertexArray::getId() const {
    return id;
}
