// g++ main.cpp -L./ -lglfw3 -lopengl32 -Wl,-subsystem,windows
// g++ main.cpp glad.c -I./includes -L./ -lglfw3 -lopengl32
// g++ main.cpp -o algae++ -std=c++11 -I./glfw-3.2.1/include -I./glew-2.1.0/include -L./glfw-3.2.1/lib -L./glew-2.1.0/lib -lglfw -lglew32s -lopengl32

// #include <glad/glad.h>
#define GLEW_STATIC
#include <glew.h>
#undef GLFW_DLL
#include <glfw3.h>

#include <stdio.h>
#include <cmath>
#include <fstream>

#include "../Socket.hpp"

const float PI = 3.14159265358979323846;
const float TAU = 2 * PI;

const int ww = 800;
const int wh = 800;

typedef struct Point {
	float x, y;
} Point;
Point mouse = {};

float vertices[42] = {
	0.0, 0.0, 1.0, 0.0, 0.0, 1.0,
	0.0, 0.0, 1.0, 1.0, 0.0, 1.0,
	0.0, 0.0, 0.0, 1.0, 0.0, 1.0,
	0.0, 0.0, 0.0, 1.0, 1.0, 1.0,
	0.0, 0.0, 0.0, 0.0, 1.0, 1.0,
	0.0, 0.0, 1.0, 0.0, 1.0, 1.0,
	0.0, 0.0, 1.0, 1.0, 1.0, 1.0
};
// {
// 	-0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // Top-left
// 	0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // Top-right
// 	0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // Bottom-right
// 	-0.5f, -0.5f, 1.0f, 1.0f, 1.0f, // Bottom-left
// 	0.0f, -0.8f, 0.0f, 0.0f, 0.0f // bottom-middle
// };

GLuint elements[] = {
	0, 1, 6,
	1, 2, 6,
	2, 3, 6,
	3, 4, 6,
	4, 5, 6,
	5, 0, 6
};

static void cursorPosCallback(GLFWwindow* win, double xpos, double ypos) {
	mouse.x = (float)xpos;
	mouse.y = (float)ypos;
	//printf("mouse, x: %f, y: %f\n", xpos, ypos);
}

std::string* readFile(const char *filename) {
	std::ifstream in(filename);
	std::string* contents = new std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
	return contents;
}

int main() {
	for (int i = 0; i < 6; ++i) {
		int n = i * 6;
		float a = i / 6.0 * TAU;
		vertices[n] = cos(a) * 0.5;
		vertices[n + 1] = sin(a) * 0.5;
		// vertices[n + 2] = sin(a / 2) * 0.5 + 0.5;
		// vertices[n + 3] = cos(a / 3 + 0.4) * 0.5 + 0.50;
		// vertices[n + 4] = sin(a * 0.7 + TAU) * 0.5 + 0.5;
		// vertices[i * 5 + 2] = 1.0;
		// vertices[i * 5 + 3] = 0.0;
		// vertices[i * 5 + 4] = 0.0;
		// printf("%f %f, %f %f %f %f\n", vertices[n], vertices[n + 1], vertices[n + 2], vertices[n + 3], vertices[n + 4], vertices[n + 5]);
	}
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	// glfwWindowHint(GLFW_SAMPLES, 32);
	// glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(ww, wh, "Algae++", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glfwSwapInterval(1);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glDisable(GL_MULTISAMPLE);

	glewExperimental = GL_TRUE;
	GLenum glewErr = glewInit();
	if (glewErr) {
		printf("glew init error: %u\n", glewErr);
	}

	glfwSetCursorPosCallback(window, cursorPosCallback);
	const GLFWvidmode* screen = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwSetWindowPos(window, screen->width / 2 - ww / 2, screen->height / 2 - wh / 2);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo); // Generate 1 buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	char* vertexSource = (char*)readFile("./vertex.glsl")->c_str();
	char* fragmentSource = (char*)readFile("./fragment.glsl")->c_str();

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, (const GLchar* const*)&vertexSource, NULL);
	glCompileShader(vertexShader);

	GLint status;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
	if (!status) {
		char buffer[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
		printf("vertex shader compilation error: %s\n", buffer);
	}

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const GLchar* const*)&fragmentSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
	if (!status) {
		char buffer[512];
		glGetShaderInfoLog(fragmentShader, 512, NULL, buffer);
		printf("fragment shader compilation error: %s\n", buffer);
	}

	GLuint shaderProgram = glCreateProgram();
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

	float bias = 0.0;
	float bx = 0.0;
	float by = 0.0;
	float r = 0.5;

	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		// glClearColor(0.2f, 0.3f, 0.3f, 0.0f);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		bias += 0.01;
		// bx = cos(-bias) * r;
		// by = sin(-bias) * r;
		bx = mouse.x / ww * 2.0 - 1.0;
		by = (mouse.y / wh * 2.0 - 1.0) * -1.0;
		for (int i = 0; i < 6; ++i) {
			int n = i * 6;
			float a = i / 6.0 * TAU + bias;
			vertices[n] = cos(a) * 0.5 + bx;
			vertices[n + 1] = sin(a) * 0.5 + by;
			// vertices[n + 5] = cos(a * 1.3) * 0.5 + 0.5;
		}
		// printf("%f\n", vertices[41 - 6]);
		vertices[6 * 6] = bx;
		vertices[6 * 6 + 1] = by;
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);
		glDrawElements(GL_TRIANGLES, 6 * 3, GL_UNSIGNED_INT, 0);

		// int error = glGetError();
		// if (error) {
		// 	printf("glError: %u\n", error);
		// 	// char buffer[512];
		// 	// glGetShaderInfoLog(fragmentShader, 512, NULL, buffer);
		// 	// printf("fragment shader compilation error: %s\n", buffer);
		// }

		glfwSwapBuffers(window);
	}

	glDeleteProgram(shaderProgram);
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);

	glDeleteBuffers(1, &vbo);

	glDeleteVertexArrays(1, &vao);

	glfwTerminate();
	return 0;
}
