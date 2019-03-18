#include "../HashTable.hpp"

#include <cassert>
#include <cstdint>
// #include <array>
#include <cstdio>


void insertionChangesLength() {
	std::printf("\tInsertion changes length... ");
	HashTable<uint8_t> table;
	assert(table.getLength() == 0);
	table.insert(7, 7);
	assert(table.getLength() == 1);
	std::puts("passed.");
}
void growing() {
	std::printf("\tGrowing... ");
	const uint32_t len = 20;
	HashTable<uint8_t> table;
	uint32_t startSize = table.getCapacity();
	for (uint8_t i = 0; i < len; ++i) {
		table.insert(i, i);
	}
	assert(table.getLength() == len);
	assert(table.getCapacity() != startSize);
	std::puts("passed.");
}
void doubleInsertionNoDuplicate() {
	std::printf("\tDouble insertion no duplicate... ");
	const uint32_t len = 10;
	HashTable<uint8_t> table;
	for (uint8_t n = 0; n < 2; ++n) {
		for (uint8_t i = 0; i < len; ++i) {
			table.insert(i * 7, i);
		}
	}
	assert(table.getLength() == len);
	std::puts("passed.");
}
void read() {
	std::printf("\tRead... ");
	const uint32_t key = 999;
	const uint32_t value = 5;
	HashTable<uint32_t> table;
	table.insert(key, value);
	assert(table.read(key) == value);
	std::puts("passed.");
}
void readAfterResize() {
	std::printf("\tRead after resize... ");
	const uint32_t keyMult = 999;
	const uint32_t valueAdd = 10000;
	const uint32_t len = 20;
	HashTable<uint32_t> table;
	for (uint8_t i = 0; i < len; ++i) {
		table.insert(i * keyMult, i + valueAdd);
	}
	for (uint8_t i = 0; i < len; ++i) {
		assert(table.read(i * keyMult) == i + valueAdd);
	}
	std::puts("passed.");
}
void has() {
	std::printf("\tHas... ");
	const uint32_t key = 67;
	HashTable<uint32_t> table;
	table.insert(23, 23);
	table.insert(90, 90);
	assert(table.has(key) == false);
	table.insert(45, 45);
	table.insert(key, key);
	table.insert(93, 93);
	assert(table.has(key));
	std::puts("passed.");
}
void removeChangesLength() {
	std::printf("\tRemove changes length... ");
	const uint32_t key = 333444;
	const uint32_t value = 9;
	HashTable<uint32_t> table;
	table.insert(key, value);
	uint32_t val = table.remove(key);
	assert(val == value);
	assert(table.getLength() == 0);
	std::puts("passed.");
}
void removeMovesElements() {
	std::printf("\tRemove moves elements... ");
	HashTable<uint32_t> table;
	for (uint32_t i = 0; i < 10; ++i) {
		table.insert(i, i);
	}
	table.remove(2);
	assert(table.has(3));
	std::puts("passed.");
}
void forEach() {
	std::printf("\tForEach... ");
	const uint32_t len = 20;
	HashTable<uint32_t> table;
	for (uint32_t i = 0; i < len; ++i) {
		table.insert(i, i);
	}
	uint32_t i = 0;
	table.forEach([&](TableNode<uint32_t>& node) {
		assert(node.payload == i);
		++i;
	});
	std::puts("passed.");
}
void filter() {
	std::printf("\tFilter... ");
	const uint32_t len = 100;
	HashTable<uint32_t> table;
	for (uint32_t i = 0; i < len; ++i) {
		table.insert(i, i);
	}
	uint32_t i = 0;
	table.filter([&](TableNode<uint32_t>& node) {
		return node.id % 3 && node.id % 5;
	});
	for (uint32_t i = 0; i < len; ++i) {
		assert(!(table.has(i) ^ (i % 3 && i % 5)));
	}
	std::puts("passed.");
}
// void insertion() {
// 	std::array<uint32_t, 20> keys = {
// 		0x930c6871,0x76139d05,0x5021e96a,0x7810c43,0x2abd14fa,
// 		0x5a8756ef,0xab8c95b6,0xc663e3,0x81f8e454,0xa4efc64a,
// 		0xe062e18a,0x43d5a60e,0x43ad03bf,0x95c70b2a,0x5b18871a,
// 		0x1a51a3a9,0x4ee5d81f,0xa7be01e5,0xecbf7e0a,0xfcd2a36e
// 	};
// 	HashTable<uint8_t> table;
// 	for (uint8_t i = 0; i < keys.getCapacity()(); ++i) {
// 		table.insert(keys[i], 100 + i);
// 	}
// 	assert(false);
// }

int main() {
	std::puts("HashTable tests running...");
	insertionChangesLength();
	growing();
	doubleInsertionNoDuplicate();
	read();
	readAfterResize();
	removeChangesLength();
	removeMovesElements();
	forEach();
	filter();
	std::puts("HashTable tests passed.\n");
}
