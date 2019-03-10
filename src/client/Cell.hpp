#pragma once

#include "../Geometry.hpp"
#include "../Vec2.hpp"
#include "../Random.hpp"

#include <vector>

class Cell : public Circle {
public:
	uint32_t id;
	uint8_t type;
	uint32_t color;
	std::vector<Vec2<float>> points;
	std::vector<float> pointsLength;
	std::vector<float> pointsVel;
	Cell() {}
	Cell(float x, float y, float r, uint32_t id, uint8_t type = 0,
		uint32_t col = 0xffffffff) : Circle(x, y, r), id(id), type(type),
		color(col)
	{
		color = col << 8 | 0xff;
	}
	void updateNumPoints(float scale, Random& prng) {
		uint16_t numPoints = (uint16_t)(r * scale);
		numPoints = std::max(numPoints, options::client::cellPointsMin);
		numPoints = std::min(numPoints, options::client::cellPointsMax);
		while (points.size() > numPoints) {
			uint16_t i = (uint16_t)(prng() * points.size());
			points.erase(points.begin() + i);
			pointsLength.erase(pointsLength.begin() + i);
			pointsVel.erase(pointsVel.begin() + i);
		}
		if (points.size() == 0) {
			points.emplace_back(x, y);
			pointsLength.push_back(r);
			pointsVel.push_back(prng() - 0.5);
		}
		while (points.size() < numPoints) {
			uint16_t i = (uint16_t)(prng() * points.size());
			points.insert(points.begin() + i, points[i]);
			pointsLength.insert(pointsLength.begin() + i, pointsLength[i]);
			pointsVel.insert(pointsVel.begin() + i, pointsVel[i]);
		}
	}
	void movePoints(Random& prng) {
		std::vector<float> pointsVelCopy = pointsVel;
		uint16_t len = points.size();
		for (uint16_t i = 0; i < len; ++i) {
			float prevVel = pointsVelCopy[(i - 1 + len) % len];
			float nextVel = pointsVelCopy[(i + 1) % len];
			float newVel = (pointsVel[i] + prng() - 0.5) * 0.7;
			newVel = std::max(std::min(newVel, 10.0f), -10.0f);
			pointsVel[i] = (prevVel + nextVel + 8 * newVel) / 10;
		}
		for (uint16_t i = 0; i < len; ++i) {
			float l = pointsLength[i];
			float prevL = pointsLength[(i - 1 + len) % len];
			float nextL = pointsLength[(i + 1) % len];
			// TODO: interaction with other cells
			l += pointsVel[i];
			l = std::max(l, 0.0f);
			l = (9 * l + r) / 10;
			pointsLength[i] = (prevL + nextL + 8 * l) / 10;

			float angle = 2 * 3.14159265 * ((float)i / len);
			Vec2<float>& p = points[i];
			p.fromAngle(angle);
			p *= l;
			p += *this;
		}
	}
	float mass() const {
		return r * r / 100;
	}
};
