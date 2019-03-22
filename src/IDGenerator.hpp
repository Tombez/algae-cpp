#pragma once

#include <cstdint>
#include <cstdio>
#include <cassert>

class IDGenerator {
public:
	const static uint32_t unusedID = ~((uint32_t)0);
	uint32_t step;
	uint32_t maxIndex;
	uint32_t index;
	IDGenerator() {
		step = 1 << 31;
		maxIndex = 1;
		index = 0;
	}
	uint32_t next() {
		if (index >= maxIndex) {
			assert(step != 1);
			step /= 2;
			maxIndex *= 2;
			index = 0;
		}
		return ((index++) * 2 + 1) * step - 1;
	}
};
