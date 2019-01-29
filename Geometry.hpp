#pragma once

class Point {
public:
	float x;
	float y;
	Point() {}
	Point(float x, float y) : x(x), y(y) {}
};

class Circle : public Point {
public:
	float r;
	Circle() {}
	Circle(float x, float y, float r) : Point(x, y), r(r) {}
};

class AABB : public Point {
public:
	float w;
	float h;
	AABB() {}
	AABB(float x, float y, float w, float h) : Point(x, y), w(w), h(h) {}
};

class Square : public Point {
public:
	float s;
	Square() {}
	Square(float x, float y, float s) : Point(x, y), s(s) {}
	bool containsCircle(Circle& c) {
		return c.x - c.r >= x && c.x + c.r <= x + s &&
			c.y - c.r >= y && c.y + c.r <= y + s;
	}
	bool overlapsAABB(AABB& aabb) {
		return x + s > aabb.x && x < aabb.x + aabb.w &&
			y + s > aabb.y && y < aabb.y + aabb.h;
	}
};
