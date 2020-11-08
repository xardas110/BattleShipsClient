#pragma once
#include <string>
#include "../include/glm/glm.hpp"
#include "Material.h"
class Shader
{
private:
	void CreateShaders(const char* vertexPath, const char* fragPath);
public:
	unsigned short ID;
	void Use();
	void Delete();
	void BindBuffer(const std::string& name, const void* buffer, const size_t size);
	void BindMat4(const std::string &name, glm::mat4 mat);
	void BindVec3(const std::string& name, glm::vec3 vec);
	void BindVec2(const std::string& name, glm::vec3 vec);
	void BindInt(const std::string& name, int id);
	void BindFloat(const std::string& name, int id);
	Shader(const char* vertexPath, const char* fragPath);
};
