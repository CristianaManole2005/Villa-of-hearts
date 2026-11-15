#version 330 core

in vec2 texCoords;
out vec4 fragColor;

uniform vec4 objectColor;
uniform sampler2D bgTexture;
uniform bool useTexture;

void main()
{
    if (useTexture)
        fragColor = texture(bgTexture, texCoords);
    else
        fragColor = objectColor;
}