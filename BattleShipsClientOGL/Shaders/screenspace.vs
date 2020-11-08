#version 460 core
layout (location = 0) in vec3 pos;   
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

void main()
{
    texCoords = texCoord;
    norm = normal;
    gl_Position = (pos.x, pos.y, pos.z, 1.0);
} 