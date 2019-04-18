#include "ResourceManagement.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int numTexturesLoaded = 0;

namespace RM {
	Texture loadTexture(const char* path)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(path, &width, &height, &channels, 4);
		if(data == nullptr)
		{
			std::cout << "Failed to load image!" << std::endl;
		}

        Texture t(data, numTexturesLoaded++, width, height);

		stbi_image_free(data);
        return t;
	}

	Shader loadShaders(const char * vertexPath, const char * fragmentPath)
	{
		std::ifstream vertex(vertexPath);
        std::ifstream fragment(fragmentPath);

        std::stringstream vStream;
        vStream << vertex.rdbuf();

        std::stringstream fStream;
        fStream << fragment.rdbuf();

        vertex.close();
        fragment.close();

        return Shader(vStream.str(), fStream.str());
	}
}
