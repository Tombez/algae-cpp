#pragma once

namespace options {
	const float viewBaseWidth = 1920;
	const float viewBaseHeight = 1080;
	namespace server {
		const float playerSpeedMultiplier = 1;
	}
	namespace client {
		const uint32_t windowWidth = 720;
		const uint32_t windowHeight = 405;
		
		const uint16_t cellPointsMin = 5;
		const uint16_t cellPointsMax = 120;

		const float gridStep = 50;
	}
}
