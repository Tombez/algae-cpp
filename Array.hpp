#pragma once

#include <stdexcept>
#include <cstdlib>
#include <cassert>
#include <utility>

template<class T>
class Array {
private:
	T* data;
	size_t length;
	size_t capacity;
public:
	Array() : data(nullptr), capacity(0), length(0) {}
	Array(size_t capacity) : capacity(capacity), length(0) {
		this->allocate();
	}
	Array(const Array<T>&) = delete;
	Array(Array<T>& b) {
		capacity = b.capacity;
		length = b.length;
		data = b.data;
		b.data = nullptr;
	}
	~Array() {
		std::free((void*)data);
		data = nullptr;
	}
	void allocate() {
		if (capacity == 0) {
			data = nullptr;
			return;
		}
		data = (T*)std::malloc(capacity * sizeof(T));
		if (data == nullptr) {
			throw std::runtime_error("Ran out of memory while allocating array.");
		}
	}
	T& operator[](size_t index) {
		assert(index < capacity);
		assert(data != nullptr);
		return data[index];
	}
	T& front() {
		return this->operator[](0);
	}
	T& back() {
		return this->operator[](capacity - 1);
	}
	void push(T thing) {
		this->operator[](length++) = thing;
	}
	template<class ...Args>
	void emplace_back(Args&& ...args) {
		assert(length < capacity);
		assert(data != nullptr);
		this->createAt(length++, std::forward<Args>(args)...);
	}
	T pop() {
		assert(length != 0);
		return this->operator[](length--);
	}
	template<class ...Args>
	void createAt(size_t index, Args&& ...args) {
		this->operator[](index).T(std::forward<Args>(args)...);
	}
	Array<T> copy(const Array<T>& b) {
		Array<T> a;
		a.capacity = b.capacity;
		a.length = b.length;
		a.allocate();
		for (size_t i = 0; i < length; i++) {
			a[i] = b[i];
		}
	}
	size_t getLength() {
		return length;
	}
	void setLength(size_t newLength) {
		assert(newLength <= capacity);
		length = newLength;
	}
	size_t getCapacity() {
		return capacity;
	}
};
