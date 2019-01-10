#include "../whichSystem.hpp"
#include "../Socket.hpp"

#if defined(unix)
#elif defined (_WIN32)
	#include "./inet_pton4.hpp"
#endif

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <vector>

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
const uint32_t MAX_PLAYERS = 9999999;

sock::Socket socky(1400);
struct sockaddr_in dest;
std::vector<Player> players;

void onReceive(struct sockaddr_in *from, uint8_t *data, uint16_t dataLen) {
	printf("message from %s:%u , opcode %u\n", inet_ntoa(from->sin_addr), ntohs(from->sin_port), data[0]);
	uint8_t opcode = data[0];

	switch (opcode) {
		case 1: // new client
			if (players.size() < MAX_PLAYERS) {
				puts("creating new client");
				players.emplace_back(from->sin_addr.s_addr, from->sin_port);
			}
			socky.sendMessage(from, (uint8_t*)"\x01\0", (uint16_t)2);
			break;
	}
}

int main() {
	memset((char*)&dest, 0, sizeof(dest));
	socky.listen(49152, onReceive);

	puts("listening for messages...");

	while(true) {
		socky.processMessages();
		for (uint32_t i = 0; i < players.size(); ++i) {
			Player& player = players[i];
			dest.sin_addr.s_addr = player.ip;
			dest.sin_port = player.port;
			socky.sendMessage(&dest, (uint8_t*)"\x02\0", 2u);
		}
	}

	return EXIT_SUCCESS;
}
