#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "RM/ResourceManagement.hpp"

const int WIDTH = 1280;
const int HEIGHT = 720;

void processInput(GLFWwindow* window, float& scale);

int main() {
	// GLFW init
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Window and context creation
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Window", nullptr, nullptr);
	if(window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

    // GLEW init
    glewInit();
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glViewport(0, 0, WIDTH, HEIGHT);
	glEnable(GL_DEPTH_TEST);

	// Shaders
    Shader basicShader = RM::loadShaders("../image-to-terrain/res/shaders/basicV.glsl",
                                         "../image-to-terrain/res/shaders/basicF.glsl");

	// Texture loading
    Texture heightmap = RM::loadTexture("../image-to-terrain/res/images/noise.png");

    int tWidth = heightmap.width;
    int tHeight = heightmap.height;

    size_t tArea = static_cast<size_t>(tWidth * tHeight);

	std::vector<float> vPos;
    vPos.reserve(tArea * 12);

	std::vector<unsigned int> indices;
    indices.reserve(tArea * 6);

	std::vector<float> texPos;
    texPos.reserve(tArea * 8);

	unsigned int index = 0;
	float scale = 10.0f;
    for(float i = 0; i < tWidth; i++)
    for(float j = 0; j < tHeight; j++)
	{
		std::vector<float> currentVPos
		{
            i, 0.0f, j,
            i + 1.0f, 0.0f, j,
            i + 1.0f, 0.0f, j + 1.0f,
            i, 0.0f, j + 1.0f,
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
            i / tWidth, j / tHeight,
            (i+1) / tWidth, j / tHeight,
            (i+1) / tWidth, (j+1) / tHeight,
            i / tWidth, (j+1) / tHeight
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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	// Indices
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	// Texture coordinates
	glGenBuffers(1, &tVBO);
	glBindBuffer(GL_ARRAY_BUFFER, tVBO);
	glBufferData(GL_ARRAY_BUFFER, texPos.size() * sizeof(float), texPos.data(), GL_STATIC_DRAW);
	
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
	glEnableVertexAttribArray(1);

	// Clean up
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	basicShader.use();
	basicShader.setInt(glGetUniformLocation(basicShader.program, "tex"), 0);

	int modelLoc = glGetUniformLocation(basicShader.program, "model");
	int projectionLoc = glGetUniformLocation(basicShader.program, "projection");
	int viewLoc = glGetUniformLocation(basicShader.program, "view");
	int scaleLoc = glGetUniformLocation(basicShader.program, "scale");

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast<float>(WIDTH) / static_cast<float>(HEIGHT), 0.1f, 1500.0f);
	basicShader.setMat4(projectionLoc, projection);

    int vertexCount = static_cast<int>(indices.size());

	while(!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		processInput(window, scale);
		basicShader.setFloat(scaleLoc, scale);

		glm::mat4 model(1.0f);
        model = glm::translate(model, { -tWidth / 2.0f, -tWidth / 6.0f, -tWidth * 1.7f });
		basicShader.setMat4(modelLoc, model);

		glm::mat4 view(1.0f);
		basicShader.setMat4(viewLoc, view);

		heightmap.bind();

		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

        glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, nullptr);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		heightmap.unbind();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window, float& scale)
{
	if(glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS)
		scale++;
	if(glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS)
		scale--;
}
