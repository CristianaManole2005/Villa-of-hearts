#version 330 core

layout(location = 0) in vec3 vertexPos;

out vec2 texCoords;

uniform mat4 transform;

void main()
{
    gl_Position = transform * vec4(vertexPos, 1.0);
    texCoords = vertexPos.xy + 0.5;
}