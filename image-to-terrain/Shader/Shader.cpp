#include "Shader.hpp"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Shader::Shader(std::string vertexSource, std::string fragmentSource)
{
	const char* vSourceC = vertexSource.c_str();
	const char* fSourceC = fragmentSource.c_str();

	// Shaders
	unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vSourceC, 0);
	glCompileShader(vertex);

	int compiled;
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &compiled);	// Check compilation status
	if(compiled != GL_TRUE)
	{
		int log_length = 0;
		char message[512];
		glGetShaderInfoLog(vertex, 512, &log_length, message);
		std::cout << "Failed to compile vertex shader!\n" << message << std::endl;
	}

	unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fSourceC, 0);
	glCompileShader(fragment);

	glGetShaderiv(vertex, GL_COMPILE_STATUS, &compiled);	// Check compilation status
	if(compiled != GL_TRUE)
	{
		int log_length = 0;
		char message[512];
		glGetShaderInfoLog(vertex, 512, &log_length, message);
		std::cout << "Failed to compile vertex shader!\n" << message << std::endl;
	}

	// Program linking
	program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);

	// Clean up
	glDetachShader(program, vertex);
	glDetachShader(program, fragment);
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::stop()
{
	glUseProgram(0);
}

void Shader::setInt(int loc, int value)
{
	glUniform1i(loc, value);
}

void Shader::setFloat(int loc, float value)
{
	glUniform1f(loc, value);
}

void Shader::setMat4(int loc, const glm::mat4& value)
{
	glUniformMatrix4fv(loc, 1, false, glm::value_ptr(value));
}

void Shader::use()
{
	glUseProgram(program);
}
