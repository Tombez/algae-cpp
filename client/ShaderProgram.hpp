#pragma once

#include "./Shader.hpp"

#include <utility>
#include <vector>

class ShaderProgram {
public:
	Shader vert;
	Shader frag;
	GLuint id;
	GLuint vao;
	GLuint vbo;
	GLuint ebo;
	std::vector<GLint> attribPositions;
	ShaderProgram() {}
	ShaderProgram(const char* vertPath, const char* fragPath,
		std::vector<std::pair<GLuint, const char*>> attributes
	) :
		vert(vertPath, GL_VERTEX_SHADER), frag(fragPath, GL_FRAGMENT_SHADER)
	{
		glGenVertexArrays(1, &vao); // vertex array objects stores attribute data
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo); // Generate 1 buffer
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

		id = glCreateProgram();
		glAttachShader(id, vert.id);
		glAttachShader(id, frag.id);
		glBindFragDataLocation(id, 0, "outColor");
		glLinkProgram(id);
		glUseProgram(id);

		GLuint attribStride = 0;
		for (uint32_t i = 0; i < attributes.size(); ++i) {
			attribStride += attributes[i].first;
		}
		GLuint offset = 0;
		for (uint32_t i = 0; i < attributes.size(); ++i) {
			std::pair<GLuint, const char*> cur = attributes[i];
			GLint attribPos = glGetAttribLocation(id, cur.second);
			attribPositions.push_back(attribPos);
			glVertexAttribPointer(attribPos, cur.first, GL_FLOAT, GL_FALSE,
				attribStride * sizeof(float), (void*)(offset * sizeof(float)));
			offset += cur.first;
			glEnableVertexAttribArray(attribPos);
		}
	}
	~ShaderProgram() {
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);

		glDetachShader(id, vert.id);
		glDetachShader(id, frag.id);
		glDeleteProgram(id);
	}
};
