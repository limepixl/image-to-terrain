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
	// GLFW init
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Window and context creation
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Window", nullptr, nullptr);
	if(window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// GLAD init
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
	glShaderSource(fragment, 1, &fragmentSource, 0);
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
	unsigned char *data = stbi_load("C:/dev/GitHub/image-to-terrain/image-to-terrain/res/images/noise.png", &width, &height, &channels, 4);
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

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	std::vector<float> vPos;
	vPos.reserve(width * height * 12);

	std::vector<unsigned int> indices;
	indices.reserve(width * height * 6);

	std::vector<float> texPos;
	texPos.reserve(width * height * 8);

	unsigned int index = 0;

	for(int i = 0; i < width; i++)
	for(int j = 0; j < height; j++)
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

		// TODO: possible wrong order of width and height
		std::vector<float> currentTexCoordinates
		{
			(float)i / width, (float)j / height,
			(float)(i+1) / width, (float)j / height,
			(float)(i+1) / width, (float)(j+1) / height,
			(float)i / width, (float)(j+1) / height
		};

		texPos.insert(texPos.end(), currentTexCoordinates.begin(), currentTexCoordinates.end());
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

	int vertexCount = (int)indices.size();

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 1500.0f);
	glUniformMatrix4fv(projectionLoc, 1, false, glm::value_ptr(projection));
	
	while(!glfwWindowShouldClose(window)) {
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 model(1.0f);
		model = glm::translate(model, { -(float)width / 2.0f, -(float)width / 6.0f, -(float)width * 1.7f });
		glUniformMatrix4fv(modelLoc, 1, false, glm::value_ptr(model));

		glm::mat4 view(1.0f);
		glUniformMatrix4fv(viewLoc, 1, false, glm::value_ptr(view));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

		glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
