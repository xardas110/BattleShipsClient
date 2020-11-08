#pragma once
#include <memory>

class Texture
{
private:
	Texture(unsigned int ID, unsigned int width, unsigned int height);
	Texture(unsigned int diffuseID, unsigned int normalID);
	unsigned int textureID, normalID;
	unsigned int width, height;
public:
	static std::shared_ptr<Texture> LoadTextureFromFile(const char* diffusepath);	
	static std::shared_ptr<Texture> LoadTextureFromFile(const char* diffusepath, const char* normalpath);//if I want to add normalMap later
	void BindTexture();
	~Texture();
};

