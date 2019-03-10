#pragma once

#include "./Vec2.hpp"

const float PI = 3.1415926;
const float TAU = 2 * PI;

class Circle : public Vec2<float> {
public:
	float r;
	Circle() {}
	Circle(float x, float y, float r) : Vec2(x, y), r(r) {}
};

class AABB : public Vec2<float> {
public:
	float w;
	float h;
	AABB() {}
	AABB(float x, float y, float w, float h) : Vec2(x, y), w(w), h(h) {}
};

class Square : public Vec2<float> {
public:
	float s;
	Square() {}
	Square(float x, float y, float s) : Vec2(x, y), s(s) {}
	bool containsCircle(Circle& c) {
		return c.x - c.r >= x && c.x + c.r <= x + s &&
			c.y - c.r >= y && c.y + c.r <= y + s;
	}
	bool overlapsAABB(AABB& aabb) {
		return x + s > aabb.x && x < aabb.x + aabb.w &&
			y + s > aabb.y && y < aabb.y + aabb.h;
	}
};
