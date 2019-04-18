#pragma once
#include "../Shader/Shader.hpp"
#include "../Texture/Texture.hpp"

namespace RM
{
	// Load a texture's data
	Texture loadTexture(const char* path);

	// Loads a vertex and fragment shader from paths
	Shader loadShaders(const char* vertexPath, const char* fragmentPath);
}