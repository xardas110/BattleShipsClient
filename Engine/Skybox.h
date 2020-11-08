#pragma once
#include "Mesh.h"
#include <vector>
#include <string>
#include <memory>
#include "Application.h"
#include "Shader.h"
class Skybox
{
private:
	std::unique_ptr<Mesh> skyMesh;
public:
	unsigned int textureId;
	Skybox();
	void Init(std::vector<std::string> textures);
	void Draw(std::shared_ptr<Shader> shader);
};

