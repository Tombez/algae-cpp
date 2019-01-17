#pragma once

// decided not to use enums so opcodes would be same across builds.
namespace opcodes {
	namespace client {
		namespace actions {
			const uint8_t split = 0b10000000;
			const uint8_t eject = 0b01000000;
		}
		const uint8_t connectReq = 0x1;
		const uint8_t disconnect = 0x2;
		const uint8_t input = 0x3;
			// float mouse x, y
			// uint8 bitset
				// bool has split
				// bool has ejected
	}
	namespace server {
		const uint8_t connectAccept = 0x1;
		const uint8_t error = 0x2;
			// ascii null terminated string "description of error"
		const uint8_t worldUpdate = 0x3;
			// uint16 eat count
			// struct eat record * (eat count)
				// uint32 eater id
				// uint32 eaten id
			// uint16 appear count
				// struct appear record * (appear count)
					// utf8 null terminated string name
					// ascii null terminated string skin name
					// uint8 r, g, b
					// uint32 id
					// uint8 bitset cell type
						// bool is pellet
						// bool is virus
						// bool is ejected
						// bool is my cell
			// uint16 update count
			// struct update record * (update count)
				// float x, y, r
			// uint16 disappear count
			// uint32 cell id * (disappear count)
	}
}
