#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const int WIDTH = 1280;
const int HEIGHT = 720;

const char *vertexSource = 
"#version 460 core\n"
"layout(location = 0) in vec3 aPos;\n"
"layout(location = 1) in vec2 aTexPos;\n"
"out vec2 texPos;\n"
"out float y;\n"
"out vec4 textureSample;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"uniform sampler2D tex;\n"
"void main() {\n"
"vec4 sampled = texture(tex, aTexPos);\n"
"float currentY = sampled.r;\n"
"gl_Position = projection * view * model * vec4(aPos.x, currentY * 10.0, aPos.z, 1.0);\n"
"y = currentY;\n"
"textureSample = sampled;\n"
"}";

const char *fragmentSource = 
"#version 460 core\n"
"in float y;\n"
"in vec4 textureSample;\n"
"out vec4 color;\n"
"void main() {\n"
"color = vec4(1.0, 1.0, 1.0, 1.0) * y;\n"
"}\n";

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Window", nullptr, nullptr);
	if(window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	glViewport(0, 0, WIDTH, HEIGHT);
	glEnable(GL_DEPTH_TEST);

	// Shaders
	unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertexSource, 0);
	glCompileShader(vertex);

	// Check compilation status
	int compiled;
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &compiled);
	if(compiled != GL_TRUE)
	{
		int log_length = 0;
		char message[1024];
		glGetShaderInfoLog(vertex, 1024, &log_length, message);
		std::cout << "Failed to compile vertex shader!\n" << message << std::endl;
	}

	unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragmentSource, 0);
	glCompileShader(fragment);

	glGetShaderiv(vertex, GL_COMPILE_STATUS, &compiled);
	if(compiled != GL_TRUE)
	{
		int log_length = 0;
		char message[1024];
		glGetShaderInfoLog(vertex, 1024, &log_length, message);
		std::cout << "Failed to compile vertex shader!\n" << message << std::endl;
	}

	// Program linking
	unsigned int program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);

	// Clean up
	glDetachShader(program, vertex);
	glDetachShader(program, fragment);
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	// Texture creation
	int width, height, channels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load("C:/dev/Projects/perlin/perlin/res/images/noise.png", &width, &height, &channels, 4);
	if(data == nullptr)
	{
		std::cout << "Failed to load image!" << std::endl;
	}

	unsigned int texture;
	glGenTextures(1, &texture);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	std::vector<float> vPos;
	std::vector<unsigned int> indices;
	std::vector<float> texPos;

	unsigned int index = 0;
	int n = 256;
	float step = (float)width / n;
	for(int i = 0; i < n; i++)
	for(int j = 0; j < n; j++)
	{
		std::vector<float> currentVPos
		{
			(float)i, 0.0f, (float)j,
			(float)i + 1.0f, 0.0f, (float)j,
			(float)i + 1.0f, 0.0f, (float)j + 1.0f,
			(float)i, 0.0f, (float)j + 1.0f,
		};

		vPos.insert(vPos.end(), currentVPos.begin(), currentVPos.end());

		std::vector<unsigned int> currentIndices
		{
			index, index + 1, index + 2,
			index + 2, index + 3, index
		};

		indices.insert(indices.end(), currentIndices.begin(), currentIndices.end());
		index += 4;

		std::vector<float> currentTexCoordinates
		{
			step * i, step * j,
			step * (i+1), step * j,
			step * (i+1), step * (j+1),
			step * i, step * (j+1)
		};

		std::vector<float> normalized;
		normalized.reserve(currentTexCoordinates.size());

		for(int it = 0; it < currentTexCoordinates.size(); it++)
		{
			normalized.push_back(currentTexCoordinates[it] / width);
		}

		texPos.insert(texPos.end(), normalized.begin(), normalized.end());
	}

	// VAO creation
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Vertex coordinates
	unsigned int VBO, EBO, tVBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vPos.size() * sizeof(float), vPos.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Indices
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	// Texture coordinates
	glGenBuffers(1, &tVBO);
	glBindBuffer(GL_ARRAY_BUFFER, tVBO);
	glBufferData(GL_ARRAY_BUFFER, texPos.size() * sizeof(float), texPos.data(), GL_STATIC_DRAW);
	
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	// Clean up
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glUseProgram(program);

	int loc = glGetUniformLocation(program, "tex");
	glUniform1i(loc, 0);

	int modelLoc = glGetUniformLocation(program, "model");
	int projectionLoc = glGetUniformLocation(program, "projection");
	int viewLoc = glGetUniformLocation(program, "view");

	while(!glfwWindowShouldClose(window)) {
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 model(1.0f);
		glUniformMatrix4fv(modelLoc, 1, false, glm::value_ptr(model));

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 1500.0f);
		glUniformMatrix4fv(projectionLoc, 1, false, glm::value_ptr(projection));

		glm::mat4 view(1.0f);
		view = glm::translate(view, { (float)-n / 2.0f, -(float)n / 8.0f, -(float)n});
		view = glm::rotate(view, glm::radians(25.0f), { 1.0f, 0.0f, 0.0f });
		view = glm::rotate(view, glm::radians(90.0f), { 0.0f, 1.0f, 0.0f });
		glUniformMatrix4fv(viewLoc, 1, false, glm::value_ptr(view));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

		glDrawElements(GL_TRIANGLES, (int)indices.size(), GL_UNSIGNED_INT, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	stbi_image_free(data);
	glfwTerminate();
	return 0;
}