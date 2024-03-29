#pragma once

#include <string>
#include <glm/glm.hpp>

using std::string;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

namespace Pancake {

    class Shader {

        private:

            unsigned int program;
            string vertex;
            string fragment;
            
            static char* loadSource(const char* filename);
            void load(const char* vertexCode, const char* fragmentCode);

        public:

            Shader(string vertexFile, string fragmentFile);
            Shader(string vertexName, string fragmentName, const char* vertexCode, const char* fragmentCode);
            ~Shader();

            void bind();
            void unbind();

            void uploadInt(string name, int value);
            void uploadFloat(string name, float value);
            void uploadVec2(string name, vec2 vector);
            void uploadVec3(string name, vec3 vector);
            void uploadVec4(string name, vec4 vector);
            void uploadMat4(string name, mat4 matrix);
            void uploadTexture(string name, int slot);
            void uploadIntArray(string name, int num, int* array);

    };

}