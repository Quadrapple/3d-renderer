#ifndef SHADER_H
#define SHADER_H

#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
    public:
        unsigned int id;

        Shader(const char *vertexShaderPath, const char *fragmentShaderPath);

        void use();

        //Uniform setters
        void setBool(const std::string &name, bool value) const;
        void setInt(const std::string &name, int value) const;
        void setFloat(const std::string &name, float value) const;

        void setMat4(const std::string &name, glm::mat4 matrix) const;

        void setVec2(const std::string &name, glm::vec2 vector) const;
        void setVec2(const std::string &name, float x, float y) const;

        void setVec3(const std::string &name, glm::vec3 vector) const;
        void setVec3(const std::string &name, float x, float y, float z) const;

        void setVec4(const std::string &name, glm::vec4 vector) const;
        void setVec4(const std::string &name, float x, float y, float z, float w) const;

        void bindUniformBlock(const std::string &name, unsigned int index) const;

    private:
        void createShaderProgram(unsigned int vertexShader, unsigned int fragmentShader);
        std::string loadFile(const char *filename);

        unsigned int compileVertexShader(const char* source);
        unsigned int compileFragmentShader(const char* source);

        void checkCompile(unsigned int id, std::string type);
        void checkLink(unsigned int id);
};

#endif
