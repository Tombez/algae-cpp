#pragma once

#include <cmath>

template<typename T=float>
class Vec2 {
public:
	T x;
	T y;
	Vec2() {}
	Vec2(T x, T y) : x(x), y(y) {}
	Vec2<T>& operator +=(const Vec2<T>& b) {
		x += b.x;
		y += b.y;
		return *this;
	}
	Vec2<T>& operator /=(T b) {
		x /= b;
		y /= b;
		return *this;
	}
	Vec2<T>& operator *=(T b) {
		x *= b;
		y *= b;
		return *this;
	}
	Vec2<T> operator *(T b) {
		return Vec2(x * b, y * b);
	}
	Vec2<T> operator -(Vec2<T> b) {
		return Vec2(x - b.x, y - b.y);
	}
	Vec2<T>& assign(const Vec2<T>& b) {
		this->x = b.x;
		this->y = b.y;
		return *this;
	}
	Vec2<T>& assign(T x, T y) {
		this->x = x;
		this->y = y;
		return *this;
	}
	T getDist() {
		return std::sqrt(x * x + y * y);
	}
};
