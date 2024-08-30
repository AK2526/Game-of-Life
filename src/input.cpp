#include <input.h>
#include <GLFW/glfw3.h>
#include <cells.h>
#include <view.h>
#include <glm/gtc/matrix_transform.hpp>

glm::vec2 prevMouseloc = glm::vec2(0, 0);

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
		updateSpeed = std::max(0, updateSpeed - 1);
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
		transformed = glm::translate(glm::mat4(1.0f), glm::vec3(0, dist, 0)) * transformed;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		transformed = glm::translate(glm::mat4(1.0f), glm::vec3(0, -dist, 0)) * transformed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		transformed = glm::translate(glm::mat4(1.0f), glm::vec3(-dist, 0, 0)) * transformed;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		transformed = glm::translate(glm::mat4(1.0f), glm::vec3(dist, 0, 0)) * transformed;

	// Check if we need to update grid while moving
	glm::vec3 m = glm::inverse(projection * transformed) * glm::vec4(prevMouseloc.x, -prevMouseloc.y, 0, 1);
	if ((int)m.x != prevHover.x || (int)m.y != prevHover.y)
	{
		if (inGrid((int)prevHover.x, (int)prevHover.y))
		{
			setCellStat((int)prevHover.x, (int)prevHover.y, IDLE);
		}

		prevHover = glm::vec2((int)m.x, (int)m.y);
		if (inGrid((int)prevHover.x, (int)prevHover.y))
		{
			setCellStat((int)prevHover.x, (int)prevHover.y, HOVER);
			if (ERASE)
				setCellCol((int)prevHover.x, (int)prevHover.y, DEAD);
			if (ADD)
				setCellCol((int)prevHover.x, (int)prevHover.y, ALIVE);
		}

	}


}

void mouse_callback(GLFWwindow* window, double xPos, double yPos)
{
	xPos = xPos / sWidth * 2 - 1;
	yPos = yPos / sHeight * 2 - 1;
	prevMouseloc = glm::vec2(xPos, yPos);
	glm::vec3 m = glm::inverse(projection * transformed) * glm::vec4(xPos, -yPos, 0, 1);
	if ((int)m.x != prevHover.x || (int)m.y != prevHover.y)
	{
		if (inGrid((int)prevHover.x, (int)prevHover.y))
		{
			setCellStat((int)prevHover.x, (int)prevHover.y, IDLE);
		}

		prevHover = glm::vec2((int)m.x, (int)m.y);
		if (inGrid((int)prevHover.x, (int)prevHover.y))
		{
			setCellStat((int)prevHover.x, (int)prevHover.y, HOVER);
			if (ERASE)
				setCellCol((int)prevHover.x, (int)prevHover.y, DEAD);
			if (ADD)
				setCellCol((int)prevHover.x, (int)prevHover.y, ALIVE);
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
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && inGrid((int)prevHover.x, (int)prevHover.y))
	{
		ADD = true;
		setCellCol((int)prevHover.x, (int)prevHover.y, ALIVE);
	}
	else
	{
		ADD = false;
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS && inGrid((int)prevHover.x, (int)prevHover.y))
	{
		ERASE = true;
		setCellCol((int)prevHover.x, (int)prevHover.y, DEAD);
	}
	else
	{
		ERASE = false;
	}

}