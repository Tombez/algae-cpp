#pragma once

#include <vector>
#include <cstdint>
#include <cstring>
// #include <algorithm>

// template<typename T>
// void function addToVector(std::vector<T>& v, void* data, uint32_t len) {
// 	if (v.size() + len > v.capacity()) {
// 		v.reserve(v.capacity() * 2);
// 	}
// 	std::memcpy(v.data() + v.size() * sizeof(T), )
// }

class Drawable {
public:
	float* va;
	uint32_t vl;
	GLuint* ea;
	uint32_t el;

	Drawable(float* va, uint32_t vl, GLuint* ea, uint32_t el) :
		va(va), vl(vl), ea(ea), el(el)
	{}
	Drawable() {}
	~Drawable() {
		// delete va;
		// delete ea;
	}
	void addToList(std::vector<float>& vbod, std::vector<GLuint>& ebod) {
		uint32_t size = vbod.size();
		uint32_t cap = vbod.capacity();
		if (cap == 0)
			cap = vl;
		while (size + vl > cap)
			cap *= 2;
		vbod.reserve(cap);
		for (uint32_t i = 0; i < vl; ++i)
			vbod.push_back(va[i]);

		size = ebod.size();
		cap = ebod.capacity();
		if (cap == 0)
			cap = el;
		while (size + el > cap)
			cap *= 2;
		ebod.reserve(cap);
		for (uint32_t i = 0; i < el; ++i)
			ebod.push_back(ea[i]);
	}
};
