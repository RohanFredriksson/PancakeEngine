#version 330 core

in vec4 fColour;
in vec2 fTexCoords;
in float fTexId;
in float fEntityId;

uniform sampler2D uTextures[8];

out vec3 colour;

void main()
{
    vec4 texColour = vec4(1, 1, 1, 1);
    switch (int(fTexId)) {
        case 1:
            texColour = fColour * texture(uTextures[1], fTexCoords);
            break;
        case 2:
            texColour = fColour * texture(uTextures[2], fTexCoords);
            break;
        case 3:
            texColour = fColour * texture(uTextures[3], fTexCoords);
            break;
        case 4:      
            texColour = fColour * texture(uTextures[4], fTexCoords);
            break;
        case 5:
            texColour = fColour * texture(uTextures[5], fTexCoords);
            break;
        case 6:
            texColour = fColour * texture(uTextures[6], fTexCoords);
            break;
        case 7:
            texColour = fColour * texture(uTextures[7], fTexCoords);
            break;
    }

    if (texColour.a < 0.5) {
        discard;
    }

    colour = vec3(fEntityId + 1, 0, 0);
}