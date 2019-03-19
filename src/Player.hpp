#pragma once

#include "./Geometry.hpp"
#include "./Vec2.hpp"
#include "./HashTable.hpp"
#include "./options.hpp"

#include <cstdint>
#include <cmath>
#include <vector>
#include <string>

template<class CellType, class SetContent=uint8_t>
class Player {
public:
	uint32_t ip;
	uint16_t port;
	// uint32_t id; // I don't think players need ids
	std::vector<CellType*> myCells;
	HashTable<SetContent> cellsByID;
	std::string name;
	std::string skin;
	Vec2<float> mouse;
	uint8_t keys;
	Player() : ip{0}, port{0} {}
	Player(uint32_t ip, uint16_t port) : ip(ip), port(port), mouse(0, 0),
		keys(0) {}
	float getScore() {
		float score = 0.0;
		for (const CellType* cell : myCells) {
			score += cell->mass();
		}
		return score;
	}
	float getViewScale() {
		float score = this->getScore();
		if (score == 0.0) {
			return 0.4; // ? ask multiogar-edited
		}
		return std::pow(std::min(64 / score, 1.0f), 0.4);
	}
	Vec2<float> getPos() {
		Vec2<float> pos(0.0, 0.0);
		for (const CellType* cell : myCells) {
			pos += *cell;
		}
		if (myCells.size() != 0) {
			pos /= myCells.size();
		}
		return pos;
	}
	AABB getView() {
		float scale = this->getViewScale();
		float w = options::viewBaseWidth / scale;
		float h = options::viewBaseHeight / scale;
		Vec2<float> pos = this->getPos();
		return AABB(pos.x - w / 2, pos.y - h / 2, w, h);
	}
};
