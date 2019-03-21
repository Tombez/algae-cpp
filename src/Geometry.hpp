#pragma once

#include "./Vec2.hpp"

const float PI = 3.1415926;
const float TAU = 2 * PI;

namespace std {
	template<typename T>
	T clamp(T v, T min, T max) {
		return v < min ? min : (v > max ? max : v);
	}
}

class Circle : public Vec2<float> {
public:
	float r;
	Circle() {}
	Circle(float x, float y, float r) : Vec2(x, y), r(r) {}
	bool overlapsCircle(const Circle& b) const {
		return this->getDistSquared(b) <= (r + b.r) * (r + b.r);
	}
	bool containsPoint(const Vec2<float>& p) const {
		return this->getDistSquared(p) <= r * r;
	}
};

class AABB : public Vec2<float> {
public:
	float w;
	float h;
	AABB() {}
	AABB(float x, float y, float w, float h) : Vec2(x, y), w(w), h(h) {}
	bool overlapsCircle(const Circle& c) const {
		const float px = std::clamp(c.x, x, x + w);
		const float py = std::clamp(c.y, y, y + h);
		return c.containsPoint(Vec2<float>(px, py));
	}
};

class Square : public Vec2<float> {
public:
	float s;
	Square() {}
	Square(float x, float y, float s) : Vec2(x, y), s(s) {}
	bool containsCircle(const Circle& c) const {
		return c.x - c.r >= x && c.x + c.r <= x + s &&
			c.y - c.r >= y && c.y + c.r <= y + s;
	}
	bool overlapsAABB(const AABB& aabb) const {
		return x + s > aabb.x && x < aabb.x + aabb.w &&
			y + s > aabb.y && y < aabb.y + aabb.h;
	}
};
