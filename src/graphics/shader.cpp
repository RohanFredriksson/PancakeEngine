#include <iostream>
#include <cstdlib>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include "pancake/graphics/shader.hpp"

using glm::value_ptr;

namespace Pancake {

    char* Shader::loadSource(const char * filename) {

        FILE* f = fopen(filename, "r");
        if (f == nullptr) {return nullptr;}

        size_t size = 16;
        size_t length = 0;
        char* source = (char*) malloc(size);

        char next;
        while (1) {
            
            next = fgetc(f);
            if (next == EOF) {
                break;
            }

            if (length >= size) {
                source = (char*) realloc(source, size * 2);
                size = size * 2;
            }

            source[length] = next;
            length++;

        }

        fclose(f);
        source = (char*) realloc(source, length + 1);
        source[length] = '\0';

        return source;   

    }

    void Shader::load(const char* vertexCode, const char* fragmentCode) {

        GLuint v, f;
        GLint success;
        GLchar infoLog[512];

        // Vertex shader compilation.
        v = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(v, 1, (char const * const *)&(vertexCode), nullptr);
        glCompileShader(v);

        // Check for compilation errors.
        glGetShaderiv(v, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(v, 512, nullptr, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << "\n";
        }

        // Fragment shader compilation.
        f = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(f, 1, (char const * const *)&(fragmentCode), nullptr);
        glCompileShader(f);

        // Check for compilation errors.
        glGetShaderiv(f, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(f, 512, nullptr, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << "\n";
        }

        // Shader program linking.
        this->program = glCreateProgram();
        glAttachShader(this->program, v);
        glAttachShader(this->program, f);
        glLinkProgram(this->program);

        // Check if any errors occurred in linking.
        glGetProgramiv(this->program, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(this->program, 512, nullptr, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << "\n";
        }

        glDeleteShader(v);
        glDeleteShader(f);

    }

    Shader::Shader(string vertexFile, string fragmentFile) {
        
        this->vertex = vertexFile;
        this->fragment = fragmentFile;

        // Load shader code
        char* vertexCode = Shader::loadSource(vertexFile.c_str());
        char* fragmentCode = Shader::loadSource(fragmentFile.c_str());
        if (vertexCode == nullptr) {std::cout << "ERROR::SHADER::COMPILE::VERTEX_SOURCE_NOT_FOUND\n";}
        if (vertexCode == nullptr) {std::cout << "ERROR::SHADER::COMPILE::FRAGMENT_SOURCE_NOT_FOUND\n";}

        // Load the shader.
        this->load((const char*) vertexCode, (const char*) fragmentCode);

        // Free the data.
        free(vertexCode);
        free(fragmentCode);
        
    }

    Shader::Shader(string vertexName, string fragmentName, const char* vertexCode, const char* fragmentCode) {

        this->vertex = vertexName;
        this->fragment = fragmentName;
        this->load(vertexCode, fragmentCode);

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
        this->bind();
        GLint location = glGetUniformLocation(this->program, name.c_str());
        glUniform1i(location, value);
    }

    void Shader::uploadFloat(string name, float value) {
        this->bind();
        GLint location = glGetUniformLocation(this->program, name.c_str());    
        glUniform1f(location, value);
    }

    void Shader::uploadVec2(string name, vec2 vector) {
        this->bind();
        GLint location = glGetUniformLocation(this->program, name.c_str());
        glUniform2f(location, vector.x, vector.y);
    }

    void Shader::uploadVec3(string name, vec3 vector) {
        this->bind();
        GLint location = glGetUniformLocation(this->program, name.c_str());
        glUniform3f(location, vector.x, vector.y, vector.z);
    }

    void Shader::uploadVec4(string name, vec4 vector) {
        this->bind();
        GLint location = glGetUniformLocation(this->program, name.c_str());
        glUniform4f(location, vector.x, vector.y, vector.z, vector.w);
    }

    void Shader::uploadMat4(string name, mat4 matrix) {
        this->bind();
        GLint location = glGetUniformLocation(this->program, name.c_str());
        glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(matrix));
    }

    void Shader::uploadTexture(string name, int slot) {
        this->bind();
        GLint location = glGetUniformLocation(this->program, name.c_str());
        glUniform1i(location, slot);
    }

    void Shader::uploadIntArray(string name, int num, int* array) {
        this->bind();
        GLint location = glGetUniformLocation(this->program, name.c_str());
        glUniform1iv(location, num, array);
    }

}