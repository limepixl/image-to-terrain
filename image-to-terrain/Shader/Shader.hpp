#pragma once
#include <string>
#include <glm/glm.hpp>

struct Shader
{
	unsigned int program;

	Shader(std::string vertexSource, std::string fragmentSource);

	void use();
	void stop();

	void setInt(int loc, int value);
	void setFloat(int loc, float value);
	void setMat4(int loc, const glm::mat4& value);
};