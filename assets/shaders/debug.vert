#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColour;

out vec3 fColour;

uniform mat4 uView;
uniform mat4 uProjection;

void main() 
{
    fColour = aColour;
    gl_Position = uProjection * uView * vec4(aPos, 1.0);
}