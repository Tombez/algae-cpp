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
	Vec2<T> operator *(T b) const {
		return Vec2(x * b, y * b);
	}
	Vec2<T> operator -(Vec2<T> b) const {
		return Vec2(x - b.x, y - b.y);
	}
	Vec2<T>& assign(const Vec2<T>& b) {
		x = b.x;
		y = b.y;
		return *this;
	}
	Vec2<T>& assign(T nx, T ny) {
		x = nx;
		y = ny;
		return *this;
	}
	Vec2<T>& fromAngle(float angle) {
		x = std::cos(angle);
		y = std::sin(angle);
		return *this;
	}
	T getDist() const {
		return std::sqrt(x * x + y * y);
	}
};
