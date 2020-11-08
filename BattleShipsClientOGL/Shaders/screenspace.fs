#version 460 core
out vec4 FragColor;
  
uniform sampler2D diffuseTex;
in vec3 fragWS;
in vec3 norm;
in vec2 texCoords;
void main()
{
	
	vec4 diffuseTex = texture(diffuseTex, texCoords);
	if (diffuseTex.a < 0.1)
		discard;
	FragColor =  diffuseTex;//vec4(finalColor.x, finalColor.y, finalColor.z, 1.0);
}
