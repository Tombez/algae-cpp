#include "../TimeStamp.hpp"
#include "../Geometry.hpp"
#include "../Vec2.hpp"
#include "../colorUtils.hpp"
#include "../Socket.hpp"
#include "../opcodes.hpp"
#include "../Buffer.hpp"
#include "../IDGenerator.hpp"
#include "../HashTable.hpp"
#include "../Random.hpp"
#include "../Player.hpp"
#include "./initGLFW.hpp"
#include "./Drawable.hpp"
#include "./Cell.hpp"
#include "./Renderer.hpp"

// #if defined(unix)
// #elif defined (_WIN32)
// 	#include "../server/inet_pton4.hpp"
// #endif

#include <cstdio>
#include <iostream>
#include <cmath>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <memory>

Vec2<float> prev(0, 0);

sock::Socket socky;
struct sockaddr_in server;

Buffer toServer(1400);
uint8_t keys = 0;
Player<Cell, Cell*> me;
Random prng;
Circle camera;
float viewScale = 1.0;
float viewportScale = 1.0;
std::vector<Cell*> cellArray;
Renderer renderer;

static void cursorPosCallback(GLFWwindow* win, double xpos, double ypos) {
	prev = me.mouse;
	me.mouse.assign((float)xpos, (float)ypos);
}
static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action != GLFW_PRESS)
		return;

	switch (key) {
		case GLFW_KEY_ESCAPE: {
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;
		}
		case GLFW_KEY_SPACE: {
			keys |= opcodes::client::actions::split;
			break;
		}
		case GLFW_KEY_W: {
			keys |= opcodes::client::actions::eject;
			break;
		}
	}
}

bool onSockError(int32_t errcode, const char* funcName) {
	if (errcode == ECONNRESET) {
		puts("server refused connection.");
	} else {
		printf("function %s failed with error code: %d\n", funcName, errcode);
	}
	// return false;
}
void onReceive(struct sockaddr_in *from, Buffer& buf) {
	uint8_t opcode = buf.read<uint8_t>();

	switch (opcode) {
		case opcodes::server::connectAccept: {
			std::puts("connection accepted");
			break;
		}
		case opcodes::server::error: {
			std::string desc = buf.read<std::string>();
			std::printf("error response from server: %s\n", desc.c_str());
			break;
		}
		case opcodes::server::worldUpdate: {
			uint16_t eatCount = buf.read<uint16_t>();
			for (uint16_t i = 0; i < eatCount; ++i) {
				uint32_t eaterID = buf.read<uint32_t>();
				uint32_t eatenID = buf.read<uint32_t>();
				// TODO: handle eaten
			}
			uint16_t updateCount = buf.read<uint16_t>();
			// std::printf("update count: %u\n", updateCount);
			for (uint16_t i = 0; i < updateCount; ++i) {
				uint32_t cellID = buf.read<uint32_t>();
				float x = buf.read<float>();
				float y = buf.read<float>();
				float r = buf.read<float>();
				uint8_t readFlags = buf.read<uint8_t>();
				uint8_t cellType = 0;
				std::string name;
				std::string skin;
				if (readFlags & opcodes::server::readFlags::type) {
					cellType = buf.read<uint8_t>();
				}
				if (readFlags & opcodes::server::readFlags::name) {
					name = buf.read<std::string>();
				}
				if (readFlags & opcodes::server::readFlags::skin) {
					skin = buf.read<std::string>();
				}
				TableNode<Cell*> node = me.cellsByID.read(cellID);
				Cell* cell;
				if (node.id == unusedID) {
					cell = new Cell(x, y, r, cellID, cellType,
						color::hueToColor(prng()));
					me.cellsByID.insert(cellID, cell);
					if (cell->type == opcodes::cellType::myCell) {
						cell->type = opcodes::cellType::player;
						me.myCells.push_back(cell);
					}
				} else {
					cell = node.payload;
					cell->x = x;
					cell->y = y;
					cell->r = r;
				}
			}

			uint16_t disappearCount = buf.read<uint16_t>();
			for (uint16_t i = 0; i < disappearCount; ++i) {
				uint32_t id = buf.read<uint32_t>();
				// TODO: handle disappeared
			}
			break;
		}
	}
}
void update(float dt) {
	camera.assign(me.getPos());
	viewScale = me.getViewScale();
	viewportScale = std::max(ww / options::viewBaseWidth, wh / options::viewBaseHeight);
	camera.r = viewScale * viewportScale;

	toServer.setIndex(0);
	toServer.write<uint8_t>(opcodes::client::input);
	float x = (me.mouse.x - ww / 2.0) / camera.r + camera.x;
	float y = (me.mouse.y - wh / 2.0) / camera.r + camera.y;
	toServer.write<float>(x);
	toServer.write<float>(y);
	toServer.write<uint8_t>(keys);
	keys = 0;
	socky.sendMessage(&server, toServer);

	me.cellsByID.forEach([&](TableNode<Cell*>& node)->void {
		cellArray.push_back(node.payload);
	});
	std::sort(cellArray.begin(), cellArray.end(), [](Cell* a, Cell* b) {
		return a->r < b->r;
	});
	for (const auto c : cellArray) {
		c->updateNumPoints(camera.r, prng);
		c->movePoints(prng);
	}
}

void draw() {
	glClearColor(17.0/255.0, 17.0/255.0, 17.0/255.0, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// grid
	renderer.color = 170 << 24 | 170 << 16 | 170 << 8 | 51;
 	uint16_t step = options::client::gridStep;
	float left = camera.x - (ww / 2) / camera.r;
	float top = camera.y - (wh / 2) / camera.r;
	float right = camera.x + (ww / 2) / camera.r;
	float bottom = camera.y + (wh / 2) / camera.r;
	float startX = left - std::fmod(left, step);
	float startY = top - std::fmod(top, step);
	for (float x = startX; x < right; x += step) {
		renderer.line(Vec2<float>(x, top), Vec2<float>(x, bottom), 1);
	}
	for (float y = startY; y < bottom; y += step) {
		renderer.line(Vec2<float>(left, y), Vec2<float>(right, y), 1);
	}

	//cells
	for (const auto c : cellArray) {
		renderer.color = c->color;
		renderer.triangleFan(*c, c->points);
	}

	glUniform2f(glsp.uniformLocations[0], camera.x, camera.y);
	glUniform2f(glsp.uniformLocations[1], camera.r / (ww / 2), -camera.r / (wh / 2));

	renderer.render();

	glfwSwapBuffers(window);
}

int main() {
	TimeStamp now;
	TimeStamp prevTime;
	float targetDt = 1.0;
	uint16_t fps = 0;
	uint32_t prevSecond = 0;
	uint32_t second = 0;

	init();
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, cursorPosCallback);

	std::printf("refresh rate %f\n", refreshRate);

	new (&socky) sock::Socket(1400, onSockError);
	socky.msgCb = onReceive;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(127 << 24 | 0 << 16 | 0 << 8 | 1);
	server.sin_port = htons((uint16_t)49152);

	toServer.setIndex(0);
	toServer.write<uint8_t>(opcodes::client::connectReq);
	toServer.write<uint8_t>(0x0);
	socky.sendMessage(&server, toServer);

	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		socky.processMessages();

		now.refresh();
		uint64_t dif = now - prevTime;
		float dt = dif / (1.0 / refreshRate * 1000000);
		if (dt >= targetDt) {
			prevTime = now;
			targetDt = std::max(1 - (dt - targetDt), 0.0f);

			second = glfwGetTime();
			++fps;
			if (second != prevSecond) {
				prevSecond = second;
				std::printf("fps %u, second %u\n", fps, second);
				fps = 0;
			}

			update(dt);
			draw();
			cellArray.clear();
			renderer.clear();
		}
	}

	toServer.setIndex(0);
	toServer.write<uint8_t>(opcodes::client::disconnect);
	toServer.write<uint8_t>(0x0);
	socky.sendMessage(&server, toServer);
	cleanup();
	return 0;
}
