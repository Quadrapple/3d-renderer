#include "shader.h"

Shader::Shader(const char *vertexShaderPath, const char *fragmentShaderPath) {
    std::string vSource = loadFile(vertexShaderPath);
    std::string fSource = loadFile(fragmentShaderPath);

    unsigned int vertexShader = compileVertexShader(vSource.c_str());
    unsigned int fragmentShader = compileFragmentShader(fSource.c_str());

    createShaderProgram(vertexShader, fragmentShader);

}

std::string Shader::loadFile(const char *filename) {
    std::ifstream sourceFile;
    std::string source;

    sourceFile.exceptions(std::ifstream::failbit | std::fstream::badbit);

    try {
        sourceFile.open(filename);

        std::stringstream sourceStream;
        sourceStream << sourceFile.rdbuf();
        source = sourceStream.str();

        sourceFile.close();

    } catch (const std::ifstream::failure& e) {
        printf("ERROR! SHADER::FAILED_FILE_READ %s\n", filename);
    }

    return source;
}

unsigned int Shader::compileVertexShader(const char* source) {
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &source, NULL);
    glCompileShader(vertexShader);

    checkCompile(vertexShader, "VERTEX");

    return vertexShader;
}

unsigned int Shader::compileFragmentShader(const char* source) {
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &source, NULL);
    glCompileShader(fragmentShader);

    checkCompile(fragmentShader, "FRAGMENT");

    return fragmentShader;
}

void Shader::checkCompile(unsigned int id, std::string type) {
    int success;
    char infoLog[512];

    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(id, 512, NULL, infoLog);
        std::cout << "ERROR! " << type << "_SHADER::COMPILE_FAIL";
        std::cout << infoLog << std::endl;
    }
}

void Shader::createShaderProgram(unsigned int vertexShader, unsigned int fragmentShader) {
    id = glCreateProgram();

    glAttachShader(id, vertexShader);
    glAttachShader(id, fragmentShader);
    glLinkProgram(id);

    checkLink(id);
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::checkLink(unsigned int id) {
    int success;
    char infoLog[512];
    glGetProgramiv(id, GL_LINK_STATUS, &success); 
    if(!success) {
        glGetProgramInfoLog(id, 512, NULL, infoLog);
        std::cout << "ERROR! SHADER_PROGRAM::LINK_FAIL";
    }
}

void Shader::use() { 
    glUseProgram(id);
}

//Uniform setters
void Shader::setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}
//--------------------------------------------------------------------
void Shader::setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}
//--------------------------------------------------------------------
void Shader::setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}
//--------------------------------------------------------------------
void Shader::setMat4(const std::string &name, glm::mat4 matrix) const {
    glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}
//--------------------------------------------------------------------
void Shader::setVec2(const std::string &name, glm::vec2 vector) const {
    glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, &vector[0]);
}

void Shader::setVec2(const std::string &name, float x, float y) const {
    glUniform2f(glGetUniformLocation(id, name.c_str()), x, y);
}
//--------------------------------------------------------------------
void Shader::setVec3(const std::string &name, glm::vec3 vector) const {
    glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &vector[0]);
}

void Shader::setVec3(const std::string &name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
}
//--------------------------------------------------------------------
void Shader::setVec4(const std::string &name, glm::vec4 vector) const {
    glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &vector[0]);
}

void Shader::setVec4(const std::string &name, float x, float y, float z, float w) const {
    glUniform4f(glGetUniformLocation(id, name.c_str()), x, y, z, w);
}
//--------------------------------------------------------------------
void Shader::bindUniformBlock(const std::string &name, unsigned int bindingPoint) const {
    unsigned int index = glGetUniformBlockIndex(id, name.c_str());
    glUniformBlockBinding(id, index, bindingPoint);
}
