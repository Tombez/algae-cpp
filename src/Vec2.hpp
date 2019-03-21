#pragma once

#include <cmath>

template<typename T=float>
class Vec2 {
public:
	T x;
	T y;
	Vec2() {}
	Vec2(T x, T y) : x(x), y(y) {}
	Vec2<T>& operator +=(const T b) {
		x += b;
		y += b;
		return *this;
	}
	Vec2<T>& operator -=(const T b) {
		x -= b;
		y -= b;
		return *this;
	}
	Vec2<T>& operator *=(const T b) {
		x *= b;
		y *= b;
		return *this;
	}
	Vec2<T>& operator /=(const T b) {
		x /= b;
		y /= b;
		return *this;
	}
	Vec2<T>& operator +=(const Vec2<T>& b) {
		x += b.x;
		y += b.y;
		return *this;
	}
	Vec2<T>& operator -=(const Vec2<T>& b) {
		x -= b.x;
		y -= b.y;
		return *this;
	}
	Vec2<T>& operator *=(const Vec2<T>& b) {
		x *= b.x;
		y *= b.y;
		return *this;
	}
	Vec2<T>& operator /=(const Vec2<T>& b) {
		x /= b.x;
		y /= b.y;
		return *this;
	}
	Vec2<T> operator +(const T b) const {
		return Vec2<T>(x + b, y + b);
	}
	Vec2<T> operator -(const T b) const {
		return Vec2<T>(x - b, y - b);
	}
	Vec2<T> operator *(const T b) const {
		return Vec2<T>(x * b, y * b);
	}
	Vec2<T> operator /(const T b) const {
		return Vec2<T>(x / b, y / b);
	}
	Vec2<T> operator +(const Vec2<T>& b) const {
		return Vec2<T>(x + b.x, y + b.y);
	}
	Vec2<T> operator -(const Vec2<T>& b) const {
		return Vec2<T>(x - b.x, y - b.y);
	}
	Vec2<T> operator *(const Vec2<T>& b) const {
		return Vec2<T>(x * b.x, y * b.y);
	}
	Vec2<T> operator /(const Vec2<T>& b) const {
		return Vec2<T>(x / b.x, y / b.y);
	}
	Vec2<T> clone() const {
		return Vec2<T>(x, y);
	}
	Vec2<T>& assign(T nx, T ny) {
		x = nx;
		y = ny;
		return *this;
	}
	Vec2<T>& assign(const Vec2<T>& b) {
		x = b.x;
		y = b.y;
		return *this;
	}
	Vec2<T>& fromAngle(const float angle) { // in radians
		x = std::cos(angle);
		y = std::sin(angle);
		return *this;
	}
	Vec2<T>& setLength(const float len) {
		return *this *= len / this->getLength();
	}
	T getLengthSquared() const {
		return x * x + y * y;
	}
	T getLength() const {
		return std::sqrt(this->getLengthSquared());
	}
	T getDistSquared(const Vec2<T>& b) const {
		return (x - b.x) * (x - b.x) + (y - b.y) * (y - b.y);
	}
	T getDist(const Vec2<T>& b) const {
		return std::sqrt(this->getDistSquared(b));
	}
};
