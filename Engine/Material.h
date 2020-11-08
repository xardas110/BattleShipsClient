#pragma once
#include "../include/glm/glm.hpp"
class Material
{
	public:
	Material();
	Material(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 spec, float shininess);
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
	
	static Material CreateMaterial(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 spec, float shininess);
};

