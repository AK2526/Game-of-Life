#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <Shader.h>
#include <stb_image.h>

#include <vector>
#include <ctime>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <input.h>
#include <cells.h>
#include <view.h>


glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
bool escPress = false;
bool SPACEPRESS = false;

glm::mat4 projection;

glm::mat4 transformed;

bool ERASE = false, ADD = false;

std::vector<int> cellNeighbours;
std::vector<int> vertices;

keyPress kp;


bool press = false;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float timeUntilUpdate = 0.0f;
int updateSpeed = 0;

glm::vec2 prevHover = glm::vec2(-5, 3);
float sWHRatio = (float)(800.0 / 600.0);
float sWidth = 800, sHeight = 600;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	sWHRatio = (float)width / (float)height;
	sWidth = (float)width;
	sHeight = (float)height;
}

void initialize();

int main()
{

	initialize();

	for (int x = 0; x < wCells; x++)
	{
		for (int y = 0; y < hCells; y++)
		{
			addPoint(vertices, x, y, DEAD);
			addPoint(vertices, x+1, y, DEAD);
			addPoint(vertices, x, y+1, DEAD);
			addPoint(vertices, x+1, y+1, DEAD);
		}
	}


	std::vector <unsigned int> indices;
	for (int i = 0; i < vertices.size() / 16 * 6 ; i++)
	{
		
		if (i % 6 < 3)
		{
			indices.push_back((i % 6) + 4 * ((int)(i / 6)));
		}
		else
		{
			indices.push_back((i % 6) + 4 * ((int)(i / 6)) - 2);
		}
	}


	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "Game of Life (Space - Step, Left Shift/Ctrl - Speed up/down) Speed: 0", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}
	
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	int xpos = (mode->width - 800) / 2;
	int ypos = (mode->height - 600) / 2;

	glfwSetWindowPos(window, xpos, ypos);

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to Initialize GLAD" << std::endl;
		return -1;
	}
	

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	Shader shade("shaders/shader.vert", "shaders/shader.frag");
	
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(int), &vertices[0], GL_DYNAMIC_DRAW);
	// Position attribute
	glVertexAttribPointer(0, 2, GL_INT, GL_FALSE, 4* sizeof(int), (void*)(0 * sizeof(int)));
	glEnableVertexAttribArray(0);

	// Color attribute
	glVertexAttribPointer(1, 1, GL_INT, GL_FALSE, 4 * sizeof(int), (void*)(2 * sizeof(int)));
	glEnableVertexAttribArray(1);

	// Stat attribute
	glVertexAttribPointer(2, 1, GL_INT, GL_FALSE, 4 * sizeof(int), (void*)(3 * sizeof(int)));
	glEnableVertexAttribArray(2);
	

	// Create EBO
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// Add image
	int width, height, nrChannels;
	unsigned char* data = stbi_load("", &width, &height, &nrChannels, 0);

	unsigned int cellTex;
	glGenTextures(1, &cellTex);

	glBindTexture(GL_TEXTURE_2D, cellTex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	stbi_set_flip_vertically_on_load(true);
	data = stbi_load("resources/cell.png", &width, &height, &nrChannels, 0);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	shade.use();
	shade.setInt("texture1", 0);


	//free image memory
	stbi_image_free(data);

	glViewport(0, 0, 800, 600);
	
	int w, h;
	glfwGetWindowSize(window, &w, &h);
	sWHRatio = (float)w / h;

	transformed = glm::mat4(1.0f);

	// Move it to the center
    transformed = glm::translate(transformed, glm::vec3(-0.5f * wCells, -0.5f * hCells, 0.0f));
	//Zoom out a bit
	transformed = glm::scale(transformed, glm::vec3(0.7, 0.7, 1));

	int nextSecond = 1;

	while (!glfwWindowShouldClose(window))
	{


		
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		if (currentFrame > nextSecond)
		{
			nextSecond++;
			std::string title = "Game of Life (Space - Step, Left Shift/Ctrl - Speed up/down) Speed: " + std::to_string(updateSpeed) + ", FPS: " + std::to_string((double)(1 / deltaTime));
			glfwSetWindowTitle(window, title.c_str());
		}

		if (updateSpeed != 0)
		{
			timeUntilUpdate -= deltaTime;
			if (timeUntilUpdate < 0)
			{
				updateCells();
				timeUntilUpdate = 1.0f/updateSpeed;
			}
		}

		processInput(window);

		glClearColor(0.0, 0.0, 0.0, 1.0); // Set state
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); // Clear using color buffer

		shade.use();

		glm::mat4 scaled = glm::mat4(1.0f);
		scaled = glm::scale(scaled, glm::vec3(0.2, 0.2, 1));

		glm::mat4 translated = glm::mat4(1.0f);
		translated = glm::translate(translated, glm::vec3(-1, -0.5,  0.0));

		projection = glm::ortho(-5.0f * sWHRatio, 5.0f * sWHRatio, -5.0f,5.0f, -1.0f, 3.0f);
		
		glm::mat4 final =  projection * transformed;

		unsigned int viewLoc = glGetUniformLocation(shade.ID, "final");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(final));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cellTex);
		/*

		glBindVertexArray(VAO);
		*/
		glBindVertexArray(VAO);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);


		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0)



		// Updating uniform variables only allwoed after using shader. 

		glfwSwapBuffers(window); // Update screen
		glfwPollEvents(); 
	}

	
	glfwTerminate();

	return 0;
}


