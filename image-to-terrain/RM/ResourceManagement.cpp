#include "ResourceManagement.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>

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

		return Texture(data, numTexturesLoaded++, width, height, channels);

		stbi_image_free(data);
	}

	Shader loadShaders(const char * vertexPath, const char * fragmentPath)
	{
		std::ifstream vertex(vertexPath);
		std::string vString;

		vertex.seekg(0, std::ios::end);
		vString.reserve(vertex.tellg());
		vertex.seekg(0, std::ios::beg);

		vString.assign((std::istreambuf_iterator<char>(vertex)), std::istreambuf_iterator<char>());

		std::ifstream fragment(fragmentPath);
		std::string fString;

		fragment.seekg(0, std::ios::end);
		fString.reserve(fragment.tellg());
		fragment.seekg(0, std::ios::beg);

		fString.assign((std::istreambuf_iterator<char>(fragment)), std::istreambuf_iterator<char>());

		return Shader(vString, fString);
	}
}
