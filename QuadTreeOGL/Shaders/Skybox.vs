#version 460 core
layout (location = 0) in vec3 pos;   

uniform mat4 MVP;
out vec3 texCoords;
void main()
{
    texCoords = pos;
    gl_Position = MVP * vec4(pos.x, pos.y, pos.z,1.0);
}  