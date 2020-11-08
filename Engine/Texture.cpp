#include "Precompiled.h"
#include "Texture.h"
#include "stb_image.h"

Texture::Texture(unsigned int ID, unsigned int width, unsigned int height)
:textureID(ID), normalID(NULL), width(width), height(height)
{
}

Texture::Texture(unsigned int diffuseID, unsigned int normalID)
    : textureID(diffuseID),normalID(normalID)
{
}

std::shared_ptr<Texture> Texture::LoadTextureFromFile(const char* path)
{
    int x, y, channels;
    unsigned int texID;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* buffer = stbi_load(path, &x, &y, &channels, NULL);

    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
   
    if (buffer)
    { 
        GLuint format, param;
        switch (channels)
        {
        case 4:
            format = GL_RGBA;
            param = GL_CLAMP_TO_EDGE;
            break;
        default:
            format = GL_RGB;
            param = GL_REPEAT;
            break;
        }
        glTexImage2D(GL_TEXTURE_2D, NULL, format, x, y, NULL, format, GL_UNSIGNED_BYTE, buffer);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, param);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, param);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
        printf("Failed to load texture data");
    stbi_image_free(buffer);
    //TODO: save this inside hash table for textureReuse
    return std::shared_ptr<Texture>(new Texture(texID, x, y));
}
std::shared_ptr<Texture> Texture::LoadTextureFromFile(const char* diffusepath, const char* normalpath)
{
    int x, y, channels;
    unsigned int texID;
    unsigned int normalID;
    unsigned char* buffer = stbi_load(diffusepath, &x, &y, &channels, NULL);

    glGenTextures(1, &texID);
    glGenTextures(1, &normalID);
    glBindTexture(GL_TEXTURE_2D, texID);
    if (buffer)
    {
        GLuint format, param;
        switch (channels)
        {
        case 4:
            format = GL_RGBA;
            param = GL_CLAMP_TO_EDGE;
            break;
        default:
            format = GL_RGB;
            param = GL_REPEAT;
            break;
        }
        glTexImage2D(GL_TEXTURE_2D, NULL, format, x, y, NULL, format, GL_UNSIGNED_BYTE, buffer);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, param);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, param);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
        printf("Failed to load texture data");
    stbi_image_free(buffer);
    buffer = stbi_load(normalpath, &x, &y, &channels, NULL);
    glBindTexture(GL_TEXTURE_2D, normalID);
    if (buffer)
    {
        GLuint format, param;
        switch (channels)
        {
        case 4:
            format = GL_RGBA;
            param = GL_CLAMP_TO_EDGE;
            break;
        default:
            format = GL_RGB;
            param = GL_REPEAT;
            break;
        }
        glTexImage2D(GL_TEXTURE_2D, NULL, format, x, y, NULL, format, GL_UNSIGNED_BYTE, buffer);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, param);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, param);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
        printf("Failed to load texture data");
    stbi_image_free(buffer);

    //TODO: save this inside hash table for textureReuse
    return std::shared_ptr<Texture>(new Texture(texID, normalID));
}
void Texture::BindTexture()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);

}

Texture::~Texture()
{
    printf("texture destructor called");
    glDeleteTextures(1, &textureID);
}
