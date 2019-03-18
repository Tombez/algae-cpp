#pragma once

#include "../Geometry.hpp"
#include "../Vec2.hpp"

class Cell : public Circle {
public:
	uint32_t id;
	uint8_t type;
	Vec2 vel; // velocity
	uint32_t eatenBy;
	Cell() {}
	Cell(float x, float y, float r, uint32_t id, uint8_t type,
		uint32_t eatenBy) : Circle(x, y, r), id(id), type(type), vel(0, 0),
		eatenBy(eatenBy) {}
	float mass() const {
		return r * r / 100;
	}
	void move(float dt) {
		*this += this->vel * dt;
	}
};
