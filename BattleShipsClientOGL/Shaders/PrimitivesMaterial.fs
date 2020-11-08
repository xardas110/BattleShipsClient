#version 460 core
out vec4 FragColor;
  

uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;
uniform float shininess;

uniform vec3 camPos;
in vec3 fragWS;
in vec3 norm;

void main()
{
		vec3 sunDir = vec3(0.0, 0.25, 1.0);
		sunDir = normalize(sunDir);
		vec3 sunColor = vec3(1.0, 1.0, 1.0);
	
		vec3 fragToCam = camPos - fragWS;
		fragToCam = normalize(fragToCam);
		float diffuseStrength = max(dot(sunDir, norm), 0);

	//pow(NDotH, material.Shininess) * (material.Shininess + 2.f) / 8.f
		vec3 halfDir = normalize(sunDir + fragToCam);
		float specularStrength = pow(max(dot(norm, halfDir), 0), shininess); 
		vec3 diffusec = sunColor * diffuseStrength * diffuse;
		vec3 spec = sunColor * specularStrength * specular;
		vec3 finalColor = clamp((diffusec + spec + ambient), 0.0, 1.0);
	

	FragColor =  vec4(finalColor, 1.f);//vec4(finalColor.x, finalColor.y, finalColor.z, 1.0);

}
