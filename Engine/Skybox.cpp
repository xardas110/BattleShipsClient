#include "Skybox.h"

Skybox::Skybox()
	:textureId(0)
{

}

void Skybox::Init(std::vector<std::string> textures)
{
	textureId = Application::LoadCubeMapTextures(textures);
	skyMesh = Mesh::CreateSkyBox();
}

void Skybox::Draw(std::shared_ptr<Shader> shader)
{
	skyMesh->Draw(textureId, 0x84C0, 0x8513);//textureSlot = 0,      textureType = cubemap
}
