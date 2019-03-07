#pragma once

#include "./Cell.hpp"

class CellColored : public Cell {
public:
	uint32_t color;
	CellColored(float x, float y, float r, uint32_t id, uint8_t type = 0,
		uint32_t col = 0xffffffff
	) :
		Cell(x, y, r, id, type)
	{
		color = col << 8 | 0xff;
	}
};
