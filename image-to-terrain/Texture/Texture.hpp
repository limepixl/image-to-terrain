#pragma once

struct Texture
{
	unsigned int texture;

	// Texture's data
	int index;
	int width;
	int height;

	Texture(unsigned char* data, int index, int width, int height, int channels);

	void bind();
	void unbind();
};