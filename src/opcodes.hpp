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
			const uint16_t eatenBy = 1 << 0;
			const uint16_t pos = 1 << 1;
			const uint16_t type = 1 << 2;
			const uint16_t name = 1 << 3;
			const uint16_t skin = 1 << 4;
		}
		const uint8_t connectAccept = 0x1;
		const uint8_t error = 0x2;
			// ascii null terminated string "description of error"
		const uint8_t worldUpdate = 0x3;
			// uint16 update count
				// struct update record * (update count)
					// uint32 id
					// uint16 bitset read flags
						// bool eatenBy
						// bool pos
						// bool type
						// bool name
						// bool skin
					// optional: uint32 eatenBy ID
					// optional: float x, y, r
					// optional: uint8 cell type
					// optional: utf8 null terminated string name
					// optional: ascii null terminated string skin
			// uint16 disappear count
				// uint32 cell id * (disappear count)
			// uint32 end of message
			const uint32_t endMessage = 1234567890;
	}
	namespace cellType {
		const uint8_t pellet = 0x1;
		const uint8_t player = 0x2;
		const uint8_t ejected = 0x3;
		const uint8_t virus = 0x4;
		const uint8_t myCell = 0x5;
	}
}
