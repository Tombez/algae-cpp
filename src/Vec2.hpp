#pragma once

#include <cmath>

class Vec2 {
public:
	float x;
	float y;
	Vec2() {}
	Vec2(float x, float y) : x(x), y(y) {}
	Vec2& operator +=(const Vec2& b) {
		x += b.x;
		y += b.y;
		return *this;
	}
	Vec2& operator /=(float b) {
		x /= b;
		y /= b;
		return *this;
	}
	Vec2& operator *=(float b) {
		x *= b;
		y *= b;
		return *this;
	}
	Vec2 operator *(float b) {
		return Vec2(x * b, y * b);
	}
	Vec2 operator -(Vec2 b) {
		return Vec2(x - b.x, y - b.y);
	}
	Vec2& assign(const Vec2& b) {
		this->x = b.x;
		this->y = b.y;
		return *this;
	}
	Vec2& assign(float x, float y) {
		this->x = x;
		this->y = y;
		return *this;
	}
	float getDist() {
		return std::sqrt(x * x + y * y);
	}
};
