#pragma once
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

struct keyPress
{
	bool shift = false;
	bool control = false;
};


extern bool ERASE, ADD;

extern keyPress kp;
extern bool firstMouse;
extern bool escPress;
extern bool SPACEPRESS;

extern bool press ;
extern float deltaTime;
extern float lastFrame;
extern float timeUntilUpdate ;
extern int updateSpeed;

extern glm::vec2 prevHover;

void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);	