#pragma once

// #include "../whichSystem.hpp"
#include "./ShaderProgram.hpp"

#include <iostream>
#include <vector>
#include <utility>

void errorCallback(int error, const char* description) {
	printf("glfw error %d, %s\n", error, description);
}

GLFWwindow* window = nullptr;
int ww = 600;
int wh = 600;
ShaderProgram glsp;
float refreshRate = 60.0;

void init() {
	if (!glfwInit()) {
		puts("glfw failed to initialize.");
		exit(1);
	}
	glfwSetErrorCallback(errorCallback);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	// glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	GLFWvidmode* videoMode = nullptr;
	if (monitor) {
		videoMode = (GLFWvidmode*)glfwGetVideoMode(monitor);

		refreshRate = videoMode->refreshRate;
		// std::cout << "refresh " << videoMode->refreshRate << "\n";
		// std::cout << "width " << videoMode->width << "\n";
		// std::cout << "height " << videoMode->height << "\n";
		glfwWindowHint(GLFW_REFRESH_RATE, videoMode->refreshRate);
		glfwWindowHint(GLFW_RED_BITS, videoMode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, videoMode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, videoMode->blueBits);
	} else {
		puts("primary monitor was null..\n");
	}

	window = glfwCreateWindow(ww, wh, "Algae++", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glfwSwapInterval(0);

	glewExperimental = GL_TRUE;
	GLenum glewErr = glewInit();
	if (glewErr) {
		printf("glew init error: %u\n", glewErr);
		exit(glewErr);
	}

	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// glEnable(GL_BLEND);
	glEnable(GL_MULTISAMPLE);

	if (videoMode != nullptr) {
		glfwSetWindowPos(window, videoMode->width / 2 - ww / 2,
			videoMode->height / 2 - wh / 2);
	}

	new (&glsp) ShaderProgram("./vertex.glsl", "./fragment.glsl",
		std::vector<std::pair<GLuint, const char*>>({
			{2, "position"},
			{4, "color"}
		}));
}

void cleanup() {
	glfwTerminate();
}
