#ifndef TEXTURE_H
#define TEXTURE_H

#include "Commons.hpp"
#include <iostream>
#include "stb_image.h"

class Texture
{
private:

public:
    unsigned int ID;

    void use(int index)
    {
        glActiveTexture(index);
        glBindTexture(GL_TEXTURE_2D, ID);
    }

	Texture(const char* path)
	{
        glGenTextures(1, &ID);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int texWidth, texHeight, nrChannels;
        unsigned char* data = stbi_load(path, &texWidth, &texHeight, &nrChannels, 0);

        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);
	}

public:
    enum Type { Diffuse, Specular };

    static std::string typeToString(Type t)
    {
        switch (t)
        {
        case Texture::Diffuse:
            return "texture_diffuse";
            break;
        case Texture::Specular:
            return "texture_specular";
            break;
        default:
            return "unknown";
            break;
        }
    }
};
#endif