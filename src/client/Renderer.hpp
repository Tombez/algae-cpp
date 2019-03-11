#pragma once

#include "../Vec2.hpp"

#include <vector>
#include <cstdint>

class Renderer {
public:
	std::vector<float> vbod;
	std::vector<GLuint> ebod;
	uint32_t color;
	Renderer() : color(0U) {}
	void triangleFan(Vec2<float> center, std::vector<Vec2<float>>& points) { // temporary function
		size_t offset = vbod.size() / 6;
		const float r = (float)(color >> 24);
		const float g = (float)(color >> 16 & 0xff);
		const float b = (float)(color >> 8 & 0xff);
		const float a = (float)(color & 0xff);
		vbod.push_back(center.x);
		vbod.push_back(center.y);
		vbod.push_back(r);
		vbod.push_back(g);
		vbod.push_back(b);
		vbod.push_back(a);
		for (uint32_t i = 0; i < points.size(); ++i) {
			Vec2<float>& p = points[i];
			vbod.push_back(p.x);
			vbod.push_back(p.y);
			vbod.push_back(r);
			vbod.push_back(g);
			vbod.push_back(b);
			vbod.push_back(a);
		}
		for (uint32_t i = 2; i <= points.size(); ++i) {
			ebod.push_back(offset);
			ebod.push_back(offset + i - 1);
			ebod.push_back(offset + i);
		}
		ebod.push_back(offset);
		ebod.push_back(offset + points.size());
		ebod.push_back(offset + 1);
	}
	void triangleStrip(std::vector<Vec2<float>>& points) {
		size_t offset = vbod.size() / 6;
		const float r = (float)(color >> 24);
		const float g = (float)(color >> 16 & 0xff);
		const float b = (float)(color >> 8 & 0xff);
		const float a = (float)(color & 0xff);
		for (uint32_t i = 0; i < points.size(); ++i) {
			Vec2<float>& p = points[i];
			vbod.push_back(p.x);
			vbod.push_back(p.y);
			vbod.push_back(r);
			vbod.push_back(g);
			vbod.push_back(b);
			vbod.push_back(a);
		}
		for (uint32_t i = 0; i + 2 < points.size(); ++i) {
			ebod.push_back(offset + i);
			ebod.push_back(offset + i + 1);
			ebod.push_back(offset + i + 2);
		}
	}
	void line(Vec2<float> start, Vec2<float> end, float width) {
		std::vector<Vec2<float>> points;
		points.reserve(4);
		width /= 2;
		float difX = end.x - start.x;
		if (difX == 0) {
			points.emplace_back(start.x - width, start.y);
			points.emplace_back(start.x + width, start.y);
			points.emplace_back(end.x - width, end.y);
			points.emplace_back(end.x + width, end.y);
		} else {
			float difY = end.y - start.y;
			if (difY == 0) {
				points.emplace_back(start.x, start.y - width);
				points.emplace_back(start.x, start.y + width);
				points.emplace_back(end.x, end.y - width);
				points.emplace_back(end.x, end.y + width);
			} else {
				Vec2<float> perp(difY, -difX);
				perp.setLength(width);
				points.push_back(start - perp);
				points.push_back(start + perp);
				points.push_back(end - perp);
				points.push_back(end + perp);
			}
		}
		this->triangleStrip(points);
	}
	void render() {
		glBufferData(GL_ARRAY_BUFFER, vbod.size() * sizeof(float), vbod.data(), GL_STREAM_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ebod.size() * sizeof(GLuint), ebod.data(), GL_STREAM_DRAW);
		glDrawElements(GL_TRIANGLES, ebod.size(), GL_UNSIGNED_INT, 0);
	}
	void clear() {
		vbod.clear();
		ebod.clear();
	}
};
