#pragma once

#include "./whichSystem.hpp"

#include <cstdint>

#if defined(unix)
	#include <chrono>
#elif defined(_WIN32)
	#include <windows.h>
#endif

class TimeStamp {
public:
	#if defined(unix)
		std::chrono::time_point<std::chrono::high_resolution_clock> time;
	#elif defined(_WIN32)
		uint64_t time;
	#endif
	TimeStamp() {
		this->refresh();
	}
	void refresh() {
		#if defined(unix)
			time = std::chrono::high_resolution_clock::now();
		#elif defined(_WIN32)
			QueryPerformanceCounter((_LARGE_INTEGER*)&time);
		#endif
	}
	uint64_t operator-(const TimeStamp& other) {
		#if defined(unix)
			return std::chrono::duration_cast<std::chrono::microseconds>(time - other.time).count();
		#elif defined(_WIN32)
			return time - other.time;
		#endif
	}
	// TimeStamp& operator=(uint32_t val) {
	// 	#if defined(unix)
	// 		time = std::chrono::time_point<std::chrono::high_resolution_clock>(val);
	// 	#elif defined(_WIN32)
	// 		time = val;
	// 	#endif
	// }
};
