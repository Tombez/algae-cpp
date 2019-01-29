#pragma once

#include <random>
#include <cstdint>

class Random {
public:
	uint32_t seed;
	std::minstd_rand rng;
	std::uniform_real_distribution<float> rand;
	Random() {
		auto rd = new std::random_device();
		seed = (*rd)();
		delete rd;
		init();
	}
	Random(uint32_t seed) : seed(seed) {
		init();
	}
	void init() {
		rng = std::minstd_rand(seed);
		rand = std::uniform_real_distribution<float>(0.0, 1.0);
	}
	template<typename T>
	T operator()(T min, T max) {
		return rand(rng) * (max - min) + min;
	}
};
