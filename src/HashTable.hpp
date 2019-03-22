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
	TableNode() : id(IDGenerator::unusedID) {}
};

template<typename T>
class HashTable {
private:
	TableNode<T> *data;
	uint32_t length;
	uint32_t capacity;
	float minLoad;
	float maxLoad;
public:
	HashTable(float minLoad = 0.25, float maxLoad = 0.7) :
		minLoad(minLoad), maxLoad(maxLoad)
	{
		length = 0;
		capacity = 2;
		data = new TableNode<T>[capacity];
	}
	~HashTable() {
		delete[] data;
	}
	uint32_t getLength() const {
		return length;
	}
	uint32_t getCapacity() const {
		return capacity;
	}
	bool has(uint32_t id) const {
		return find(id) != IDGenerator::unusedID;
	}
	void insert(uint32_t id, T item) {
		uint32_t i = findFrom(hash(id), [&](TableNode<T> &node)->bool {
			return node.id == id || node.id == IDGenerator::unusedID;
		});
		assert(i != capacity);
		TableNode<T> &node = data[i];
		if (node.id == id) {
			node.payload = item;
			return;
		}
		++length;
		node.id = id;
		node.payload = item;
		if ((float)length / capacity >= maxLoad) {
			resize(capacity * 2);
		}
	}
	T remove(uint32_t id) {
		uint32_t i = find(id);
		assert(i != IDGenerator::unusedID);
		T elem = data[i].payload;
		removeElement(i);
		checkResizeDown();
		return elem;
	}
	void erase(uint32_t id) {
		uint32_t i = find(id);
		if (i == IDGenerator::unusedID) return;
		removeElement(i);
		checkResizeDown();
	}
	T& read(uint32_t id) {
		uint32_t i = find(id);
		assert(i != IDGenerator::unusedID);
		return data[i].payload;
	}
	uint32_t hash(uint32_t id) const {
		return id / (uint32_t)(~((uint32_t)0) / (double)capacity);
	}
	template<class CB>
	void forEach(CB callback) {
		for (uint32_t i = 0; i < capacity; ++i) {
			if (data[i].id != IDGenerator::unusedID) {
				callback(data[i]);
			}
		}
	}
	template<class CB>
	void filter(CB callback) {
		for (uint32_t i = 0; i < capacity;) {
			if (data[i].id != IDGenerator::unusedID) {
				if (!callback(data[i])) {
					removeElement(i);
					continue; // very important that it skips increment
				}
			}
			++i;
		}
		uint32_t newSize = nextLargerPowerOfTwo(length * 2);
		if (newSize < capacity) {
			resize(newSize);
		}
	}
private:
	void resize(uint32_t newSize) {
		TableNode<T>* oldData = data;
		uint32_t oldSize = capacity;
		data = new TableNode<T>[newSize];
		length = 0;
		capacity = newSize;
		for (uint32_t i = 0; i < oldSize; ++i) {
			if (oldData[i].id != IDGenerator::unusedID) {
				this->insert(oldData[i].id, oldData[i].payload);
			}
		}
		delete[] oldData;
	}
	template<class CB>
	uint32_t findFrom(uint32_t i, CB callback) const {
		assert(i < capacity);
		uint32_t end = i;
		const uint32_t mask = capacity - 1;
		do {
			if (callback(data[i])) {
				return i;
			}
			i = (i + 1) & mask;
		} while (i != end);
		return capacity;
	}
	uint32_t find(uint32_t id) const {
		uint32_t i = hash(id);
		uint32_t end = i;
		const uint32_t mask = capacity - 1;
		do {
			if (data[i].id == id) {
				return i;
			} else if (data[i].id == IDGenerator::unusedID) {
				return IDGenerator::unusedID;
			}
			i = (i + 1) & mask;
		} while (i != end);
		std::puts("Impossibility of HashTable being full while finding.");
		assert(false);
	}
	void removeElement(uint32_t i) {
		--length;
		uint32_t end = i;
		uint32_t mask = capacity - 1;
		uint32_t next = (i + 1) & mask;
		do {
			if (next == hash(data[next].id)) {
				data[i].id = IDGenerator::unusedID;
				break;
			}
			data[i] = data[next];
			if (data[next].id == IDGenerator::unusedID) break;
			i = next;
			next = (i + 1) & mask;
		} while (i != end);
	}
	void checkResizeDown() {
		if ((float)length / capacity < minLoad && capacity > 2) {
			resize(capacity / 2);
		}
	}
};
