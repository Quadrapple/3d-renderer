#include "context.h"

#include <stdexcept>
#include <iostream>

Context::Context(std::string title, glm::uvec2 size) {
    initGLFW();
    createWindow(title, size);
    initGLAD();

    windowSize = size;
    plswork[0] = this;
}

Context& Context::getContext() {
    return *plswork[0];
}

void Context::initGLFW() {
    if(!glfwInit()) {
        throw std::runtime_error("GLFW init failed");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, MAJOR_VER);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, MINOR_VER);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void Context::initGLAD() {
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("GLAD init failed");
    }
}

void Context::createWindow(std::string title, glm::uvec2 size) {
    window = glfwCreateWindow(size.x, size.y, "Asteroids", NULL, NULL);
    glfwMakeContextCurrent(window);

    if(!window) {
        throw std::runtime_error("Window creation failed");
    }
}

void Context::bindBuffer(GLenum target, unsigned int id) {
    if(targets[target] != id) {
        glBindBuffer(target, id);
        targets[target] = id;
    }
}

void Context::bindBufferBase(GLenum target, unsigned int index, unsigned int id) {
    BindingPoint bp{target, index};
    if(bindingPoints[bp] != id) {
        glBindBufferBase(target, index, id);
        bindingPoints[bp] = id;
    }
}

void Context::bindVertexArray(unsigned int id) {
    if(boundVaoID != id) {
        glBindVertexArray(id);
        boundVaoID = id;
    }
}

void Context::enable(GLenum option) {
    if(enabled[option] == false) {
        glEnable(option);
        enabled[option] = true;
    }
}

void Context::disable(GLenum option) {
    if(enabled[option] == true) {
        glDisable(option);
        enabled[option] = false;
    }
}

void Context::useProgram(unsigned int id) {
    if(usedShaderID != id) {
        glUseProgram(id);
        usedShaderID = id;
    }
}
