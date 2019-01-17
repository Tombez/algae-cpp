#include "../whichSystem.hpp"
#include "../Socket.hpp"
#include "../TimeStamp.hpp"
#include "../opcodes.hpp"
#include "../Buffer.hpp"

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
Buffer toClient(1400);

bool onSockError(int32_t errcode, uint8_t* funcName) {
	std::printf("function %s failed with error code: %d\n", funcName, errcode);
}
void onReceive(struct sockaddr_in *from, uint8_t *data, uint16_t dataLen) {
	Buffer buf;
	buf.index = 0;
	buf.size = dataLen;
	buf.data = data;

	uint8_t opcode = buf.read<uint8_t>();
	// std::printf("message from %s:%u , opcode %u\n",
	// 	inet_ntoa(from->sin_addr), ntohs(from->sin_port), opcode);

	switch (opcode) {
		case opcodes::client::connectReq:
			if (players.size() < MAX_PLAYERS) {
				std::puts("creating new client");
				players.emplace_back(from->sin_addr.s_addr, from->sin_port);
				toClient.index = 0;
				toClient.write<uint8_t>(opcodes::server::connectAccept);
				toClient.write<uint8_t>(0x0);
				socky.sendMessage(from, toClient.data, toClient.index);
			} else {
				toClient.index = 0;
				toClient.write<uint8_t>(opcodes::server::error);
				toClient.write<uint8_t*>((uint8_t*)"server already full");
				socky.sendMessage(from, toClient.data, toClient.index);
			}
			break;
		case opcodes::client::disconnect:
			for (uint32_t i = 0; i < players.size(); ++i) {
				Player& p = players[i];
				if (p.ip == from->sin_addr.s_addr &&
					p.port == from->sin_port)
				{
					players.erase(players.begin() + i);
					break;
				}
			}
			std::puts("client disconnected");
			break;
		case opcodes::client::input:
			float x = buf.read<float>();
			float y = buf.read<float>();
			uint8_t keys = buf.read<uint8_t>();
			if (keys & opcodes::client::actions::split) {
				std::puts("client split");
			}
			if (keys & opcodes::client::actions::eject) {
				std::puts("client ejected");
			}
			break;
	}
	buf.data = nullptr;
}

void update(float dt) {
	for (uint32_t i = 0; i < players.size(); ++i) {
		Player& player = players[i];
		dest.sin_addr.s_addr = player.ip;
		dest.sin_port = player.port;

		toClient.index = 0;
		toClient.write<uint8_t>(opcodes::server::worldUpdate);
		toClient.write<uint8_t>(0x0);
		socky.sendMessage(&dest, toClient.data, toClient.index);
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
