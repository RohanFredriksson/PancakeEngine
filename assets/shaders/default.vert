#version 330 core
layout (location=0) in vec3 aPos;
layout (location=1) in vec4 aColour;
layout (location=2) in vec2 aTexCoords;
layout (location=3) in float aTexId;
layout (location=4) in float aEntityId;

uniform mat4 uProjection;
uniform mat4 uView;

out vec4 fColour;
out vec2 fTexCoords;
out float fTexId;
out float fEntityId;

void main()
{
    fColour = aColour;
    fTexCoords = aTexCoords;
    fTexId = aTexId;
    fEntityId = aEntityId;
    gl_Position = uProjection * uView * vec4(aPos, 1.0);
}
