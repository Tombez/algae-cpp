#pragma once

#include "./Array.hpp"

#include <cstdint>
#include <cstdio>
#include <string>
#include <cassert>
#include <utility>

class Buffer {
private:
	uint16_t index;
public:
	Array<uint8_t> data;
	Buffer() {}
	Buffer(uint16_t size) : data(size), index(0) {}
	Buffer(const Buffer&) = delete;
	Buffer(Buffer& b) {
		index = b.index;
		data = std::move(b.data);
	}
	template<typename T>
	T read() {
		assert(index + sizeof(T) <= data.getLength());
		T result = 0;
		for (uint8_t i = 0; i < sizeof(T); ++i, ++index) { // compiler unroll?
			result |= data[index] << i * 8;
		}
		return result;
	}
	template<typename T>
	void write(T val) {
		assert(index + sizeof(T) <= data.getCapacity());
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
	uint16_t getIndex() {
		return index;
	}
	void setIndex(uint16_t newIndex) {
		assert(newIndex <= data.getLength());
		index = newIndex;
	}
};

template<>
float Buffer::read<float>() {
	uint32_t result = this->read<uint32_t>();
	return *(float*)&result;
}
template<>
std::string Buffer::read<std::string>() {
	uint16_t start = index;
	assert(index < data.getLength());
	while (data[index] != '\0') {
		++index;
		assert(index < data.getLength());
	}
	++index;
	return std::string((const char*)&data[start]);
}
template<>
void Buffer::write<float>(float val) {
	this->write<uint32_t>(*(uint32_t*)&val);
}
template<>
void Buffer::write<std::string>(std::string str) {
	uint8_t* c_str = (uint8_t*)str.c_str();
	uint16_t i = 0;
	do {
		assert(index < data.getCapacity());
		data[index++] = c_str[i++];
	} while (c_str[i] != '\0');
}
