#pragma once

#if defined(unix) || defined(__unix) || defined(__unix__) || defined(__linux__)
	#if not defined(unix)
		#define unix 1
	#endif
#elif defined(_WIN32)
	#define WIN32_LEAN_AND_MEAN
#else
	#error "Expected to compile on a unix or windows system... :("
#endif
