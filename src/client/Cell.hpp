#pragma once

#include "../Geometry.hpp"

class Cell : public Circle {
public:
	uint32_t id;
	uint8_t type;
	Cell() {}
	Cell(float x, float y, float r, uint32_t id, uint8_t type = 0) :
		Circle(x, y, r), id(id), type(type) {}
	float mass() const {
		return r * r / 100;
	}
};
