#pragma once

// decided not to use enums so opcodes would be same across builds.
namespace opcodes {
	namespace client {
		namespace actions {
			const uint8_t split = 1 << 7;
			const uint8_t eject = 1 << 6;
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
		namespace readFlags {
			const uint8_t type = 1 << 7;
			const uint8_t name = 1 << 6;
			const uint8_t skin = 1 << 5;
		}
		const uint8_t connectAccept = 0x1;
		const uint8_t error = 0x2;
			// ascii null terminated string "description of error"
		const uint8_t worldUpdate = 0x3;
			// uint16 eat count
				// struct eat record * (eat count)
					// uint32 eater id
					// uint32 eaten id
			// uint16 update count
				// struct update record * (update count)
					// uint32 id
					// float x, y, r
					// uint8 bitset read flags
						// bool type
						// bool name
						// bool skin
					// uint8 cell type
					// utf8 null terminated string name
					// ascii null terminated string skin
			// uint16 disappear count
				// uint32 cell id * (disappear count)
	}
	namespace cellType {
		const uint8_t pellet = 0x1;
		const uint8_t player = 0x2;
		const uint8_t ejected = 0x3;
		const uint8_t virus = 0x4;
		const uint8_t myCell = 0x5;
	}
}
