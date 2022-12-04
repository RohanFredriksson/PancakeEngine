#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include "graphics/primitives/shader.hpp"

using glm::value_ptr;

Shader::Shader(string vertex, string fragment) {
    
    this->vertex = vertex;
    this->fragment = fragment;

    // Load vertex file code.
    char* vertexCode;
    long vertexSize;
    FILE* vertexFile = fopen(vertex.c_str(), "rb");
    if (vertexFile == NULL) {
        printf("ERROR::SHADER::COMPILE::VERTEX_SOURCE_NOT_FOUND\n");
    }
    fseek(vertexFile, 0, SEEK_END);
    vertexSize = ftell(vertexFile);
    vertexCode = (char*) malloc(vertexSize);
    fread(vertexCode, vertexSize, 1, vertexFile);
    fclose(vertexFile);

    // Load fragment file code.
    char* fragmentCode;
    long fragmentSize;
    FILE* fragmentFile = fopen(fragment.c_str(), "rb");
    if (fragmentFile == NULL) {
        printf("ERROR::SHADER::COMPILE::FRAGMENT_SOURCE_NOT_FOUND\n");
    }
    fseek(fragmentFile, 0, SEEK_END);
    fragmentSize = ftell(fragmentFile);
    fragmentCode = (char*) malloc(fragmentSize);
    fread(fragmentCode, fragmentSize, 1, fragmentFile);
    fclose(fragmentFile);

    GLuint v, f;
    GLint success;
    GLchar infoLog[512];

    // Vertex shader compilation.
    v = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(v, 1, (char const * const *)&(vertexCode), NULL);
    glCompileShader(v);

    // Check for compilation errors.
    glGetShaderiv(v, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(v, 512, NULL, infoLog);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s", infoLog);
    }

    // Fragment shader compilation.
    f = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(f, 1, (char const * const *)&(fragmentCode), NULL);
    glCompileShader(f);

    // Check for compilation errors.
    glGetShaderiv(f, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(f, 512, NULL, infoLog);
        printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s", infoLog);
    }

    // Shader program linking.
    this->program = glCreateProgram();
    glAttachShader(this->program, v);
    glAttachShader(this->program, f);
    glLinkProgram(this->program);

    // Check if any errors occurred in linking.
    glGetProgramiv(this->program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(this->program, 512, NULL, infoLog);
        printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s", infoLog);
    }

    // Free the data.
    free(vertexCode);
    free(fragmentCode);
    glDeleteShader(v);
    glDeleteShader(f);
    
}

Shader::~Shader() {
    glDeleteProgram(this->program);
}

void Shader::bind() {
    glUseProgram(this->program);
}

void Shader::unbind() {
    glUseProgram(0);
}

void Shader::uploadInt(string name, int value) {
    GLint location = glGetUniformLocation(this->program, name.c_str());
    this->bind();
    glUniform1i(location, value);
}

void Shader::uploadFloat(string name, float value) {
    GLint location = glGetUniformLocation(this->program, name.c_str());
    this->bind();
    glUniform1f(location, value);
}

void Shader::uploadVec2(string name, vec2 vector) {
    GLint location = glGetUniformLocation(this->program, name.c_str());
    this->bind();
    glUniform2f(location, vector.x, vector.y);
}

void Shader::uploadVec3(string name, vec3 vector) {
    GLint location = glGetUniformLocation(this->program, name.c_str());
    this->bind();
    glUniform3f(location, vector.x, vector.y, vector.z);
}

void Shader::uploadVec4(string name, vec4 vector) {
    GLint location = glGetUniformLocation(this->program, name.c_str());
    this->bind();
    glUniform4f(location, vector.x, vector.y, vector.z, vector.w);
}

void Shader::uploadMat4(string name, mat4 matrix) {
    GLint location = glGetUniformLocation(this->program, name.c_str());
    this->bind();
    glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(matrix));
}

void Shader::uploadTexture(string name, int slot) {
    GLint location = glGetUniformLocation(this->program, name.c_str());
    this->bind();
    glUniform1i(location, slot);
}

void Shader::uploadIntArray(string name, int num, int* array) {
    GLint location = glGetUniformLocation(this->program, name.c_str());
    this->bind();
    glUniform1iv(location, num, array);
}