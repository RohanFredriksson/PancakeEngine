#version 330 core

in vec4 fColour;
in vec2 fTexCoords;
in float fTexId;
in float fEntityId;

uniform sampler2D uTextures[8];

out vec4 colour;

void main()
{
    switch (int(fTexId)) {
        case 0:
            colour = fColour;
            break;
        case 1:
            colour = fColour * texture(uTextures[1], fTexCoords);
            break;
        case 2:
            colour = fColour * texture(uTextures[2], fTexCoords);
            break;
        case 3:
            colour = fColour * texture(uTextures[3], fTexCoords);
            break;
        case 4:      
            colour = fColour * texture(uTextures[4], fTexCoords);
            break;
        case 5:
            colour = fColour * texture(uTextures[5], fTexCoords);
            break;
        case 6:
            colour = fColour * texture(uTextures[6], fTexCoords);
            break;
        case 7:
            colour = fColour * texture(uTextures[7], fTexCoords);
            break;
    }
}