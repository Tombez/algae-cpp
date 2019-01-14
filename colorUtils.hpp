#pragma once

#include <algorithm>
#include <cmath>

namespace color {
	// https://en.wikipedia.org/wiki/HSL_and_HSV#From_HSV
	const auto h2cc = [](float x)->float {
		return std::min(1.0, std::max(0.0,
			std::abs(x - (std::floor(x / 6.0) * 6.0) - 3.0) - 1.0
		));
	};
	uint32_t hueToColor(float hue) { // 0.0 to 1.0
		return (uint8_t)(h2cc(hue * 6.0) * 255) << 16 |
			(uint8_t)(h2cc(hue * 6.0 + 4.0) * 255) << 8 |
			(uint8_t)(h2cc(hue * 6.0 + 8.0) * 255);
	}
}
