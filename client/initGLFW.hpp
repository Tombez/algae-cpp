#pragma once

// #include "../whichSystem.hpp"

// #include <glad/glad.h>
#define GLEW_STATIC
#include <glew.h>
// #undef GLFW_DLL
#include <glfw3.h>

#include <string>
#include <fstream>
#include <iostream>

GLuint createShader(GLenum shaderType, char* filepath) {
	std::ifstream in(filepath);
	std::string content = std::string((std::istreambuf_iterator<char>(in)),
		std::istreambuf_iterator<char>());
	const char* source = content.c_str();

	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, (const GLchar* const*)&source, NULL);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		const uint16_t bufSize = 512;
		char* buffer = new char[bufSize];
		glGetShaderInfoLog(shader, bufSize, NULL, buffer);
		printf("%u shader compilation error: %s\n", shaderType, buffer);
		delete buffer;
	}
	return shader;
}

void errorCallback(int error, const char* description) {
	printf("glfw error %d, %s\n", error, description);
}

GLFWwindow* window = nullptr;
int ww = 600;
int wh = 600;
GLuint shaderProgram;
GLuint vertexShader;
GLuint fragmentShader;
GLuint vao;
GLuint vbo;
GLuint ebo;

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
	if (monitor) {
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		// std::cout << "refresh " << mode->refreshRate << "\n";
		// std::cout << "width " << mode->width << "\n";
		// std::cout << "height " << mode->height << "\n";
		glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
		glfwWindowHint(GLFW_RED_BITS, mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
		// ww = mode->width;
		// wh = mode->height;
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

	const GLFWvidmode* screen = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwSetWindowPos(window, screen->width / 2 - ww / 2, screen->height / 2 - wh / 2);

	glGenVertexArrays(1, &vao); // vertex array objects stores attribute data
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo); // Generate 1 buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	vertexShader = createShader(GL_VERTEX_SHADER, (char*)"./vertex.glsl");
	fragmentShader = createShader(GL_FRAGMENT_SHADER, (char*)"./fragment.glsl");

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glBindFragDataLocation(shaderProgram, 0, "outColor");
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
	glEnableVertexAttribArray(posAttrib);

	GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
}

void cleanup () {
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);

	glDetachShader(shaderProgram, vertexShader);
	glDetachShader(shaderProgram, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteProgram(shaderProgram);

	glfwTerminate();
}
