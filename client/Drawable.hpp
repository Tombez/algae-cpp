#pragma once

#include <cstdint>

class Drawable {
public:
	float* va;
	uint32_t vl;
	GLuint* ea;
	uint32_t el;

	Drawable(float* va, uint32_t vl, GLuint* ea, uint32_t el) :
		va(va), vl(vl), ea(ea), el(el)
	{}
	Drawable() {}
	~Drawable() {
		// delete va;
		// delete ea;
	}
};
