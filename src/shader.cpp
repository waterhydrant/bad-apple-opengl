#include "shader.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

namespace {
    void checkCompileErrors(unsigned int shader) {
        int success;
        char infoLog[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, sizeof(infoLog), NULL, infoLog);
            std::cerr << "Shader compilation failed: " << infoLog << '\n';
            std::abort();
        }
    }

    void checkLinkerErrors(unsigned int program) {
        int success;
        char infoLog[512];
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(program, sizeof(infoLog), NULL, infoLog);
            std::cerr << "Program linking failed: " << infoLog << '\n';
            std::abort();
        }
    }
}

Shader::Shader(const char *vertexPath, const char *fragmentPath) {
    std::ifstream vertexFile(vertexPath), fragmentFile(fragmentPath);
    if (!vertexFile.is_open()) {
        std::cerr << "Could not open vertex shader at " << vertexPath << std::endl;
        std::abort();
    }
    if (!fragmentFile.is_open()) {
        std::cerr << "Could not open fragment shader at " << fragmentPath << std::endl;
        std::abort();
    }

    std::stringstream vertexBuf, fragmentBuf;
    vertexBuf << vertexFile.rdbuf();
    fragmentBuf << fragmentFile.rdbuf();

    std::string vertexStr = vertexBuf.str();
    std::string fragmentStr = fragmentBuf.str();

    const char *vertexSrc = vertexStr.c_str();
    const char *fragmentSrc = fragmentStr.c_str();

    unsigned int vertex, fragment;

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexSrc, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex);

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentSrc, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment);

    id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    glLinkProgram(id);
    checkLinkerErrors(id);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::~Shader() {
    glDeleteProgram(id);
}

void Shader::use() {
    glUseProgram(id);
}
