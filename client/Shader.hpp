#pragma once

// #include <glad/glad.h>
#define GLEW_STATIC
#include "./glew-2.1.0/include/glew.h"
// #undef GLFW_DLL
#include "./glfw-3.2.1/include/glfw3.h"

#include <string>
#include <fstream>

class Shader {
public:
	GLuint id;
	GLenum type;
	Shader() {}
	Shader(const char* filepath, GLenum type) : type(type) {
		std::ifstream in(filepath);
		std::string content = std::string((std::istreambuf_iterator<char>(in)),
			std::istreambuf_iterator<char>());
		const char* source = content.c_str();

		id = glCreateShader(type);
		glShaderSource(id, 1, (const GLchar* const*)&source, NULL);
		glCompileShader(id);

		GLint status;
		glGetShaderiv(id, GL_COMPILE_STATUS, &status);
		if (status != GL_TRUE) {
			const uint16_t bufSize = 512;
			char* buffer = new char[bufSize];
			glGetShaderInfoLog(id, bufSize, NULL, buffer);
			printf("%u shader compilation error: %s\n", type, buffer);
			delete[] buffer;
		}
	}
	~Shader() {
		glDeleteShader(id);
	}
};
