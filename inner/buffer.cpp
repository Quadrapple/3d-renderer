#include "buffer.h"

#include "state.h"
#include <iostream>

Buffer::Buffer() : id(0), size(0) {}

Buffer::Buffer(GLenum type, unsigned int size, const void* data) {
    glGenBuffers(1, &id);
    this->type = type;
    bufferData(size, data);
}

void Buffer::bind() {
    State::getContext().bindBuffer(type, id);
}

void Buffer::bindAs(GLenum target) {
    State::getContext().bindBuffer(target, id);
}

void Buffer::bindBufferBase(unsigned int index) {
    State::getContext().bindBufferBase(type, index, id);
}

void Buffer::bindBufferBaseAs(GLenum target, unsigned int index) {
    State::getContext().bindBufferBase(target, index, id);
}

void Buffer::bufferData(unsigned int size, const void* data) {
    bind();
    this->size = size;
    glBufferData(type, size, data, GL_STATIC_DRAW);
}

void Buffer::bufferSubData(unsigned int offset, unsigned int size, const void* data) {
    bind();
    glBufferSubData(type, offset, size, data);
}

unsigned int Buffer::getId() {
    return id;
}

unsigned int Buffer::getSize() {
    return size;
}
