#include "Material.h"

Material::Material()
	:ambient(0.f), diffuse(0.f), specular(0.f), shininess(0.f)
{
}

Material::Material(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 spec, float shininess)
	:ambient(ambient), diffuse(diffuse), specular(spec), shininess(shininess)
{

}

Material Material::CreateMaterial(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 spec, float shininess)
{
	Material material(ambient, diffuse, spec, shininess);
	return material;
}
