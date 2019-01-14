#include "../whichSystem.hpp"
#include "../Socket.hpp"
#include "../TimeStamp.hpp"

#if defined(unix)
#elif defined (_WIN32)
	#include "./inet_pton4.hpp"
#endif

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <vector>
#include <algorithm>

class Player;

class PlayerCell {
public:
	float x;
	float y;
	float r;
	Player* parent;
};

class Player {
public:
	uint32_t ip;
	uint16_t port;
	uint32_t id;
	std::vector<PlayerCell*> cells;
	Player(uint32_t ip, uint16_t port) : ip(ip), port(port) {}
};

const uint16_t PORT = 49152;
const uint32_t MAX_PLAYERS = 9999;

sock::Socket socky;
struct sockaddr_in dest;
std::vector<Player> players;
TimeStamp now;
TimeStamp prevTime;

bool onSockError(int32_t errcode, uint8_t* funcName) {
	std::printf("function %s failed with error code: %d\n", funcName, errcode);
}
void onReceive(struct sockaddr_in *from, uint8_t *data, uint16_t dataLen) {
	std::printf("message from %s:%u , opcode %u\n", inet_ntoa(from->sin_addr), ntohs(from->sin_port), data[0]);
	uint8_t opcode = data[0];

	switch (opcode) {
		case 1: // new client
			if (players.size() < MAX_PLAYERS) {
				std::puts("creating new client");
				players.emplace_back(from->sin_addr.s_addr, from->sin_port);
				socky.sendMessage(from, (uint8_t*)"\x01\0", (uint16_t)2);
			} else {
				socky.sendMessage(from, (uint8_t*)"\x02server already full\0", (uint16_t)21);
			}
			break;
		case 2: // client disconnect
			for (uint32_t i = 0; i < players.size(); ++i) {
				Player& p = players[i];
				if (p.ip == /*(uint32_t)*/from->sin_addr.s_addr &&
					p.port == /*(uint16_t)*/from->sin_port)
				{
					players.erase(players.begin() + i);
					break;
				}
			}
			std::puts("client disconnected");
			break;
	}
}

void update(float dt) {
	for (uint32_t i = 0; i < players.size(); ++i) {
		Player& player = players[i];
		dest.sin_addr.s_addr = player.ip;
		dest.sin_port = player.port;
		socky.sendMessage(&dest, (uint8_t*)"\x03\0", (uint16_t)2);
	}
}

int main() {
	memset((char*)&dest, 0, sizeof(dest));
	dest.sin_family = AF_INET;

	new (&socky) sock::Socket(1400, onSockError);
	socky.listen(49152, onReceive);

	puts("listening for messages...");

	float targetDt = 1.0;

	while(true) {
		socky.processMessages();

		now.refresh();
		uint64_t dif = now - prevTime;
		float dt = dif / (1.0 / 50.0 * 1000000);
		if (dt >= targetDt) {
			prevTime = now;
			targetDt = std::max(1 - (dt - targetDt), 0.0f);

			update(dt);
		}
	}

	return 0;
}
