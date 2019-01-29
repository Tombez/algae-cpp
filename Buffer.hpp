#pragma once

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cassert>

class Buffer {
public:
	uint32_t index;
	uint16_t size;
	uint8_t* data;
	Buffer() {}
	Buffer(uint16_t size) : size(size), index(0) {
		data = new uint8_t[size];
	}
	template<typename T>
	T read() {
		assert(index + sizeof(T) <= size);
		T result = 0;
		for (uint8_t i = 0; i < sizeof(T); ++i, ++index) { // compiler unroll?
			result |= data[index] << i * 8;
		}
		return result;
	}
	template<typename T>
	void write(T val) {
		assert(index + sizeof(T) <= size);
		for (uint8_t i = 0; i < sizeof(T); ++i, ++index) { // compiler unroll?
			data[index] = (val >> i * 8) & 0xff; // do I need the & 0xff?
		}
	}
	template<typename T>
	void writeAt(T val, uint16_t i) {
		uint16_t realIndex = index;
		index = i;
		this->write<T>(val);
		index = realIndex;
	}
	~Buffer() {
		delete data;
	}
};

template<>
float Buffer::read<float>() {
	uint32_t result = this->read<uint32_t>();
	return *(float*)&result;
}
template<>
uint8_t* Buffer::read<uint8_t*>() {
	uint8_t* start = data + index * sizeof(uint8_t);
	while (data[index] != '\0') {
		assert(index < size);
		++index;
	}
	++index;
	uint8_t* end = data + index * sizeof(uint8_t);
	uint16_t len = (end - start); // in bytes
	uint8_t* result = new uint8_t[len / sizeof(uint8_t)];
	std::memcpy(result, start, len);
	return result;
}
template<>
void Buffer::write<float>(float val) {
	this->write<uint32_t>(*(uint32_t*)&val);
}
template<>
void Buffer::write<uint8_t*>(uint8_t* str) {
	uint16_t i = 0;
	while (true) {
		assert(index < size);
		if (str[i] == '\0') {
			break;
		}
		data[index++] = str[i++];
	}
	data[index++] = '\0';
}
