#pragma once

#include "./IDGenerator.hpp"

#include <cstdint>
#include <cstdio>
#include <utility>

// From Bit Twiddling Hacks
template<typename T>
T nextLargerPowerOfTwo(T x) {
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	return x + 1;
}

template<typename T>
class TableNode {
public:
	uint32_t id;
	T payload;
	TableNode() : id(unusedID) {}
};

template<typename T>
class HashTable {
public:
	TableNode<T> *data;
	uint32_t length;
	uint32_t size;
	HashTable() {
		length = 0;
		size = 2;
		data = new TableNode<T>[size];
	}
	template<class CB>
	uint32_t findFrom(uint32_t i, CB callback) {
		assert(i < size);
		uint32_t end = i;
		do {
			if (callback(data[i])) {
				return i;
			}
			i = (i + 1) & (size - 1);
		} while (i != end);
		return size;
	}
	uint32_t find(uint32_t id) {
		uint32_t i = hash(id);
		uint32_t end = (i + size - 1) & (size - 1);
		for (; i != end; i = (i + 1) & (size - 1)) {
			if (data[i].id == id) {
				return i;
			} else if (data[i].id == unusedID) {
				return unusedID;
			}
		}
		std::puts("Impossibility of HashTable being full while finding.");
		assert(false);
	}
	bool has(uint32_t id) {
		return find(id) != unusedID;
	}
	void insert(uint32_t id, T item) {
		if ((float)(length + 1) / size >= 0.7) {
			changeSize(size * 2);
		}
		++length;
		uint32_t i = hash(id);
		uint32_t end = (i + size - 1) & (size - 1);
		for (; i != end; i = (i + 1) & (size - 1)) {
			if (data[i].id == unusedID) {
				data[i].id = id;
				data[i].payload = item;
				return;
			}
		}
		std::puts("Impossibility of HashTable being full while inserting.");
		assert(false);
	}
	T remove(uint32_t id) {
		if ((float)(length - 1) / size < 0.25 && size > 2) {
			changeSize(size / 2);
		}
		--length;
		T node;
		uint32_t i = hash(id);
		uint32_t end = (i + size - 1) & (size - 1);
		for (; i != end; i = (i + 1) & (size - 1)) {
			if (data[i].id == id) {
				node = data[i];
				data[i].id = unusedID;
				return node;
			} else if (data[i].id == unusedID) {
				node.id = unusedID;
				return node;
			}
		}
		std::puts("Impossibility of HashTable being full while removing.");
		assert(false);
	}
	T read(uint32_t id) {
		uint32_t i = find(id);
		T node;
		if (i == unusedID) {
			node.id = unusedID;
		} else {
			node = data[i];
		}
		return node;
	}
	void changeSize(uint32_t newSize) {
		TableNode<T>* oldData = data;
		uint32_t oldSize = size;
		data = new TableNode<T>[newSize];
		length = 0;
		size = newSize;
		for (uint32_t i = 0; i < oldSize; ++i) {
			if (oldData[i].id != unusedID) {
				insert(oldData[i].id, oldData[i].payload);
			}
		}
		delete oldData;
	}
	uint32_t hash(uint32_t id) {
		return id / (uint32_t)(~((uint32_t)0) / (double)size);
	}
	template<class CB>
	void forEach(CB callback) {
		for (uint32_t i = 0; i < size; ++i) {
			if (data[i].id != unusedID) {
				callback(&data[i]);
			}
		}
	}
	template<class CB>
	void filter(CB callback) {
		for (uint32_t i = 0; i < size; ++i) {
			if (data[i].id != unusedID) {
				if (!callback(&data[i])) {
					data[i].id = unusedID;
					--length;
				}
			}
		}
		uint32_t newSize = nextLargerPowerOfTwo(length * 2);
		if (newSize < size) {
			changeSize(newSize);
		}
	}
};
