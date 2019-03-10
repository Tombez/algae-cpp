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
