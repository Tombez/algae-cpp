#include "../whichSystem.hpp"
#include "../Socket.hpp"
#include "../TimeStamp.hpp"
#include "../opcodes.hpp"
#include "../Buffer.hpp"
#include "../Geometry.hpp"
#include "../Random.hpp"
#include "../LooseQuadTree.hpp"
#include "../IDGenerator.hpp"
#include "../HashTable.hpp"
#include "../Cell.hpp"

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

class PlayerCell : public Cell {
public:
	Player* parent;
	PlayerCell() {}
	PlayerCell(float x, float y, float r, uint32_t id, Player* parent) :
		Cell(x, y, r, id, opcodes::cellType::player), parent(parent) {}
};

class Player {
public:
	uint32_t ip;
	uint16_t port;
	// uint32_t id; // I don't think players need ids
	std::vector<Cell*> cells;
	HashTable<uint8_t> cellSet;
	std::string name;
	std::string skin;
	Player(uint32_t ip, uint16_t port) : ip(ip), port(port) {}
};

const uint16_t PORT = 49152;
const uint32_t MAX_PLAYERS = 9999;
const uint16_t MAX_PACKET_SIZE = 1400;
const uint8_t QT_MAX_OBJECTS = 15;
const float MAP_SIZE = 512.0;
const float MIN_MAP = -MAP_SIZE / 2;
const float MAX_MAP = MAP_SIZE / 2;
const uint32_t PELLET_MIN_COUNT = 20;
const float PELLET_MIN_R = 1.0;
const float PELLET_MAX_R = 2.0;
const float TPS = 5.0;

sock::Socket socky;
struct sockaddr_in dest;
std::vector<Player> players;
TimeStamp now;
TimeStamp prevTime;
Buffer toClient(MAX_PACKET_SIZE);
LooseQuadTree<Cell> qt(MIN_MAP, MIN_MAP, MAP_SIZE, QT_MAX_OBJECTS);
uint32_t cellID = 0;
std::vector<Cell*> cells;
uint32_t pelletCount = 0;
Random prng;
IDGenerator idGen;
uint32_t frame = 0;
HashTable<Cell*> cellsByID;

bool onSockError(int32_t errcode, const char* funcName) {
	std::printf("function %s failed with error code: %d\n", funcName, errcode);
}
void onReceive(struct sockaddr_in *from, Buffer &buf) {
	uint8_t opcode = buf.read<uint8_t>();
	// std::printf("message from %s:%u , opcode %u\n",
	// 	inet_ntoa(from->sin_addr), ntohs(from->sin_port), opcode);

	switch (opcode) {
		case opcodes::client::connectReq: {
			if (players.size() < MAX_PLAYERS) {
				std::puts("creating new client");
				players.emplace_back(from->sin_addr.s_addr, from->sin_port);
				toClient.setIndex(0);
				toClient.write<uint8_t>(opcodes::server::connectAccept);
				toClient.write<uint8_t>(0x0);
				socky.sendMessage(from, toClient);
			} else {
				toClient.setIndex(0);
				toClient.write<uint8_t>(opcodes::server::error);
				toClient.write<std::string>(std::string("server already full"));
				socky.sendMessage(from, toClient);
			}
			break;
		}
		case opcodes::client::disconnect: {
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
		}
		case opcodes::client::input: {
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
		default: {
			printf("unknown opcode %u\n", opcode);
			break;
		}
	}
}

void update(float dt) {
	uint8_t localFrame = frame & 0xff;
	for(; pelletCount < PELLET_MIN_COUNT; ++pelletCount) {
		const float x = prng(MIN_MAP, MAX_MAP);
		const float y = prng(MIN_MAP, MAX_MAP);
		const float r = prng(PELLET_MIN_R, PELLET_MAX_R);
		const uint32_t id = idGen.next();
		Cell* pellet = new Cell(x, y, r, id);
		pellet->type = opcodes::cellType::pellet;
		cells.push_back(pellet);
		cellsByID.insert(pellet->id, pellet);
	}
	// TODO: add viruses
	// TODO: player input handling
	// TODO: move cells
	for (uint32_t i = 0; i < cells.size(); ++i) {
		qt.insertCircle(cells[i]);
	}
	// TODO: collision handling
	for (uint32_t i = 0; i < players.size(); ++i) {
		Player& player = players[i];
		dest.sin_addr.s_addr = player.ip;
		dest.sin_port = player.port;

		// TODO: take into account view size
		AABB view = AABB(-1920.0 / 2, -1080.0 / 2, 1920.0, 1080.0);
		for (uint16_t i = 0; i < player.cells.size(); ++i) {
			Cell* cell = player.cells[i];
			view.x += cell->x / player.cells.size();
			view.y += cell->y / player.cells.size();
		}
		std::vector<Cell*> onScreen = qt.getVerletList(view);

		toClient.setIndex(0);
		toClient.write<uint8_t>(opcodes::server::worldUpdate);

		toClient.write<uint16_t>(0x0); // eat count
		// TODO: eaten cells

		// uint16_t updateCountIndex = toClient.index;
		uint16_t updateCount = onScreen.size();// 0;
		std::printf("update count %u\n", updateCount);
		toClient.write<uint16_t>(updateCount);
		for (uint32_t i = 0; i < onScreen.size(); ++i) {
			Cell* cell = onScreen[i];
			toClient.write<uint32_t>(cell->id);
			toClient.write<float>(cell->x);
			toClient.write<float>(cell->y);
			toClient.write<float>(cell->r);
			uint16_t readFlagsIndex = toClient.getIndex();
			uint8_t readFlags = 0;
			toClient.write<uint8_t>(readFlags);
			if (!player.cellSet.has(cell->id)) {
				readFlags |= opcodes::server::readFlags::type;
				toClient.write<uint8_t>(cell->type);
				if (cell->type == opcodes::cellType::player) {
					PlayerCell* cell = (PlayerCell*)cell;
					if (!cell->parent->name.empty()) {
						readFlags |= opcodes::server::readFlags::name;
						toClient.write<std::string>(cell->parent->name);
					}
					if (!cell->parent->skin.empty()) {
						readFlags |= opcodes::server::readFlags::skin;
						toClient.write<std::string>(cell->parent->skin);
					}
				}
			}
			toClient.writeAt<uint8_t>(readFlags, readFlagsIndex);
			player.cellSet.insert(cell->id, localFrame);
		}
		// toClient.writeAt<uint16_t>(updateCount, updateCountIndex);

		uint16_t disappearCount = 0;
		uint16_t disappearCountIndex = toClient.getIndex();
		toClient.write<uint16_t>(disappearCount);
		player.cellSet.forEach([&](TableNode<uint8_t>* node) {
			if (node->payload != localFrame) {
				++disappearCount;
				toClient.write<uint32_t>(node->id);
			}
		});
		toClient.writeAt<uint16_t>(disappearCount, disappearCountIndex);
		player.cellSet.filter([&](TableNode<uint8_t>* node) {
			return node->payload == localFrame;
		});

		toClient.write<uint8_t>(0x0); // end of message
		socky.sendMessage(&dest, toClient);
	}
	qt.clear();
	// TODO: remove dead cells
	++frame;
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
		float dt = dif / (1.0 / TPS * 1000000);
		if (dt >= targetDt) {
			prevTime = now;
			targetDt = std::max(1 - (dt - targetDt), 0.0f);

			update(dt);
		}
	}

	return 0;
}
