#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <Shader.h>
#include <stb_image.h>

#include <vector>
#include <ctime>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct keyPress
{
	bool shift = false;
	bool control = false;
};

keyPress kp;

bool press = false;
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
float timeUntilUpdate = 0.0f;
int updateSpeed = 0;

bool firstMouse = true;
bool escPress = false;
bool SPACEPRESS = false;
std::vector<int> vertices;
std::vector<int> cellNeighbours;

const int wCells = 1000;
const int hCells = 1000;

const int DYING = 1, DEAD = 2, BORN = 3, ALIVE = 4;
const int IDLE = 0, HOVER = 1;

bool ERASE = false, ADD = false;

glm::vec2 prevHover = glm::vec2(-5, 3);


float sWHRatio = 800.0 / 600.0;
float sWidth = 800, sHeight = 600;


glm::mat4 projection;

glm::mat4 transformed;


glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

void updateCells();

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	sWHRatio = (float)width / (float)height;
	sWidth = width;
	sHeight = height;
}

int getCellInd(int x, int y)
{
	return x * hCells + y % hCells;
}

bool inGrid(int x, int y)
{
	if (0 <= x && x < wCells && 0 <= y  && y< hCells)
	{
		return true;
	}
	return false;
}

void setCellCol(int x, int y, int col)
{
	int i = getCellInd(x, y) * 16;
	vertices[i + 2] = col;
	vertices[i + 6] = col;
	vertices[i + 10] = col;
	vertices[i + 14] = col;

	
	glBufferSubData(GL_ARRAY_BUFFER, i * sizeof(int), sizeof(int) * 16, &vertices[i]);
}

void updateCol(int i, int col)
{
	vertices[i] = col;
	vertices[i + 4] = col;
	vertices[i + 8] = col;
	vertices[i + 12] = col;
}

void setCellStat(int x, int y, int col)
{

	int i = getCellInd(x, y) * 16;

	//std::cout << i << std::endl;
	vertices[i + 3] = col;
	vertices[i + 7] = col;
	vertices[i + 11] = col;
	vertices[i + 15] = col;
	
	glBufferSubData(GL_ARRAY_BUFFER, i * sizeof(int), sizeof(int) * 16, &vertices[i]);
}

void printMat4(glm::mat4 m)
{
	std::cout << "| " << m[0][0] << "\t" << m[0][1] << "\t" << m[0][2] << "\t" << m[0][3] << " |\n";
	std::cout << "| " << m[1][0] << "\t" << m[1][1] << "\t" << m[1][2] << "\t" << m[1][3] << " |\n";
	std::cout << "| " << m[2][0] << "\t" << m[2][1] << "\t" << m[2][2] << "\t" << m[2][3] << " |\n";
	std::cout << "| " << m[3][0] << "\t" << m[3][1] << "\t" << m[3][2] << "\t" << m[3][3] << " |\n";
	std::cout << "\n";
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE)
		escPress = false;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && !escPress)
	{
		escPress = true;
		if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		SPACEPRESS = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE && SPACEPRESS)
	{
		SPACEPRESS = false;
		updateCells();
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && !kp.control)
	{
		kp.control = true;
		updateSpeed = std::max(0, updateSpeed-1);
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE)
	{
		kp.control = false;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && !kp.shift)
	{
		kp.shift = true;
		updateSpeed++;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		kp.shift = false;
	}
		

	float dist = deltaTime * 10;

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		transformed = glm::translate(glm::mat4(1.0f), glm::vec3(0,dist, 0 )) * transformed;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		transformed = glm::translate(glm::mat4(1.0f), glm::vec3(0, -dist, 0)) * transformed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		transformed = glm::translate(glm::mat4(1.0f), glm::vec3(-dist, 0, 0)) * transformed;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		transformed = glm::translate(glm::mat4(1.0f), glm::vec3(dist, 0, 0)) * transformed;

}

void mouse_callback(GLFWwindow* window, double xPos, double yPos)
{
	xPos = xPos/sWidth*2	- 1;
	yPos = yPos / sHeight*2 - 1;
	glm::vec3 m = glm::inverse(projection * transformed) * glm::vec4(xPos, -yPos, 0, 1);
	if ((int)m.x != prevHover.x || (int)m.y != prevHover.y)
	{
		if (inGrid(prevHover.x, prevHover.y))
		{
			setCellStat(prevHover.x, prevHover.y, IDLE);
		}
			
		prevHover = glm::vec2((int)m.x, (int)m.y);
		if (inGrid(prevHover.x, prevHover.y))
		{
			setCellStat(prevHover.x, prevHover.y, HOVER);
			if (ERASE)
				setCellCol(prevHover.x, prevHover.y, DEAD);
			if (ADD)
				setCellCol(prevHover.x, prevHover.y, ALIVE);
		}
			
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (yoffset > 0.3)
		yoffset = 0.5;
	else if (yoffset < -0.3)
		yoffset = -0.5;
	yoffset *= 10;

	yoffset *= deltaTime;
	yoffset = std::pow(10, yoffset);
	transformed = glm::scale(glm::mat4(1.0f), glm::vec3(yoffset, yoffset, yoffset)) * transformed;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && inGrid(prevHover.x, prevHover.y))
	{
		ADD = true;
		setCellCol(prevHover.x, prevHover.y, ALIVE);
	}
	else
	{
		ADD = false;
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS && inGrid(prevHover.x, prevHover.y))
	{
		ERASE = true;
		setCellCol(prevHover.x, prevHover.y, DEAD);
	}
	else
	{
		ERASE = false;
	}

}

void addPoint(std::vector<int>& v, int x1, int y1, int stat)
{
	v.push_back( x1);
	v.push_back( y1);
	v.push_back(stat);
	v.push_back(IDLE);
}

void updateNeighboursSimple(int index)
{
	cellNeighbours[index - 1] ++;
	cellNeighbours[index + 1]++;

	index = index - 1 - hCells;
	cellNeighbours[index++] ++;
	cellNeighbours[index++] ++;
	cellNeighbours[index];

	index += hCells + hCells;
	cellNeighbours[index--]++;
	cellNeighbours[index--]++;
	cellNeighbours[index]++;
	
}

void updateNeighboursComplex(int index)
{
	int x = index / hCells;
	int y = index % hCells;

	int left = (x - 1 + wCells) % wCells;
	int right = (x + 1 + wCells) % wCells;
	int up = (y + 1 + hCells) % hCells;
	int down = (y - 1 + hCells) % hCells; 

	cellNeighbours[getCellInd(left, up)]++;
	cellNeighbours[getCellInd(x, up)]++;
	cellNeighbours[getCellInd(right, up)]++;
	cellNeighbours[getCellInd(left, y)]++;
	cellNeighbours[getCellInd(right, y)]++;
	cellNeighbours[getCellInd(left, down)]++;
	cellNeighbours[getCellInd(x, down)]++;
	cellNeighbours[getCellInd(right, down)]++;

}

void updateCells()
{

	cellNeighbours = std::vector<int>(wCells * hCells, 0);

	int i = 2;
	int j;

	for (j = 0; j < hCells; j++)
	{
		if (vertices[i] == ALIVE || vertices[i] == BORN)
			updateNeighboursComplex(j);
		i += 16;
	}

	for (int k = 1; k < wCells-1; k++)
	{
		if (vertices[i] == ALIVE || vertices[i] == BORN)
			updateNeighboursComplex(j);
		j++;
		i += 16;
		for (int l = 1; l < hCells - 1; l++)
		{
			if (vertices[i] == ALIVE || vertices[i] == BORN)
				updateNeighboursSimple(j);
			j++;
			i += 16;
		}
		if (vertices[i] == ALIVE || vertices[i] == BORN)
			updateNeighboursComplex(j);
		j++;
		i += 16;
	}
	for (int k = 0; k < hCells; k++)
	{
		if (vertices[i] == ALIVE || vertices[i] == BORN)
			updateNeighboursComplex(j);
			j++;
			i += 16;
	}



	j = -1;

	for (int i = 2; i < vertices.size(); i += 16)
	{
		
		j++;
		// IF you are a dead cell
		if ((vertices[i] == DEAD || vertices[i] == DYING) && cellNeighbours[j] == 3)
		{
			updateCol(i, BORN);
			continue;
		}
		else if (vertices[i] == DYING)
		{
			updateCol(i, DEAD);
			continue;
		}

		if ((vertices[i] == ALIVE || vertices[i] == BORN) && (cellNeighbours[j] < 2 || cellNeighbours[j] > 3))
		{
			updateCol(i, DYING);
			continue;
		}
		else if (vertices[i] == BORN)
			updateCol(i, ALIVE);
		
	}
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(int), &vertices[0]);

}


void printVBOData(GLuint vbo, GLsizei size) {
	// Bind the VBO
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	// Map the buffer to client memory
	int* mappedData = static_cast<int*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY));
	if (mappedData) {
		// Print the data
		for (GLsizei i = 0; i < size; ++i) {
			std::cout << mappedData[i] << " ";
		}
		std::cout << std::endl;

		// Unmap the buffer
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}
	else {
		std::cerr << "Failed to map VBO" << std::endl;
	}

	// Unbind the VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


int main()
{

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
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


	GLFWwindow* window = glfwCreateWindow(800, 600, "First", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}

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

	if (data) { std::cout << "texture 1 successful \n"; };

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	shade.use();
	shade.setInt("texture1", 0);


	//free image memory
	stbi_image_free(data);

	/*
	
	// Color attribute
	// glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	// glEnableVertexAttribArray(1);
	// Texture attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);

	/*
	
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);



	// NEW TEXTURE
	unsigned int texture2;
	glGenTextures(1, &texture2);

	glBindTexture(GL_TEXTURE_2D, texture2);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	


	shade.setFloat("mixval", 0.5);


	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));


	glm::mat4 view = glm::mat4(1.0f);



	glm::mat4 projection;
	projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

	unsigned int transformLoc = glGetUniformLocation(shade.ID, "transform");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model));
	*/

	glViewport(0, 0, 800, 600);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	
	int w, h;
	glfwGetWindowSize(window, &w, &h);
	sWHRatio = (float)w / h;

	transformed = glm::mat4(1.0f);
	printMat4(transformed);

	while (!glfwWindowShouldClose(window))
	{
		
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		if (updateSpeed != 0)
		{
			timeUntilUpdate -= deltaTime;
			if (timeUntilUpdate < 0)
			{
				updateCells();
				timeUntilUpdate = 1.0/updateSpeed;
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
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		//glBindBuffer(GL_ARRAY_BUFFER, VBO);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//glDrawArrays(GL_TRIANGLES, 0, 1);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);


		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0)



		// Updating uniform variables only allwoed after using shader. 

		glfwSwapBuffers(window); // Update screen
		glfwPollEvents(); 
	}

	
	glfwTerminate();

	return 0;
}


