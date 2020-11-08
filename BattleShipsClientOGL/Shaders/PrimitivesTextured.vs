#version 460 core
layout (location = 0) in vec3 pos;   
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
uniform mat4 MVP;
uniform mat4 worldspace;
out vec3 norm;
out vec3 fragWS;
out vec2 texCoords;
void main()
{
    texCoords = texCoord;
    norm = normal;
    fragWS = (worldspace * vec4(pos.xyz, 1.0)).xyz;
    gl_Position = MVP * vec4(pos.x, pos.y, pos.z,1.0);
}  