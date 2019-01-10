#include "../TimeStamp.hpp"
#include "../Geometry.hpp"
#include "../colorUtils.hpp"
#include "../Socket.hpp"
#include "./initGLFW.hpp"
#include "./Drawable.hpp"

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

const float PI = 3.14159265358979323846;
const float TAU = 2 * PI;
const int numPoints = 6;

Point mouse(0, 0);
Point prev(0, 0);

Drawable hexagon;

static void cursorPosCallback(GLFWwindow* win, double xpos, double ypos) {
	prev = mouse;
	mouse.x = (float)xpos;
	mouse.y = (float)ypos;
}
static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action != GLFW_PRESS)
		return;

	switch (key) {
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;
	}
}

float bias = 0.0;
float bx = 0.0;
float by = 0.0;
float r = 0.5;

TimeStamp now;
TimeStamp prevTime;

uint16_t fps = 0;
uint32_t prevSecond = 0;
uint32_t second = 0;

std::vector<float> vbod;
std::vector<GLuint> ebod;

sock::Socket socky(1400);
struct sockaddr_in server;

void onReceive(struct sockaddr_in *from, uint8_t *data, uint16_t dataLen) {
	// printf("message from %s:%u , opcode %u\n", inet_ntoa(from->sin_addr), ntohs(from->sin_port), data[0]);
	uint8_t opcode = data[0];

	switch (opcode) {
		case 1: // accepted connection
			puts("connection accepted");
			break;
		case 2: // error
			printf("error response from server: %s\n", data + 1 * sizeof(uint8_t));
			break;
		case 3: // world update
			puts("world update");
			break;
	}
}

void update(float dt) {
	bias += 0.02 * dt;
	bx = mouse.x / ww * 2.0 - 1.0;
	by = (mouse.y / wh * 2.0 - 1.0) * -1.0;
	for (int i = 0; i < numPoints; ++i) {
		float a = (float)i / numPoints * TAU + bias;
		hexagon.va[i * 6] = cos(a) * r + bx;
		hexagon.va[i * 6 + 1] = sin(a) * r + by;
	}
	hexagon.va[numPoints * 6] = bx;
	hexagon.va[numPoints * 6 + 1] = by;
}

void draw() {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glBufferData(GL_ARRAY_BUFFER, vbod.size() * sizeof(float), vbod.data(), GL_STREAM_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ebod.size() * sizeof(GLuint), ebod.data(), GL_STREAM_DRAW);
	glDrawElements(GL_TRIANGLES, ebod.size(), GL_UNSIGNED_INT, 0);

	glfwSwapBuffers(window);
}

int main() {
	init();
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, cursorPosCallback);

	float* va = new float[(numPoints + 1) * 6];
	for (uint32_t i = 0; i < numPoints; ++i) {
		float p = (float)i / numPoints;
		va[i * 6 + 2] = color::h2cc(p * 6.0);
		va[i * 6 + 3] = color::h2cc(p * 6.0 + 4.0);
		va[i * 6 + 4] = color::h2cc(p * 6.0 + 8.0);
		va[i * 6 + 5] = 1.0;
	}
	va[numPoints * 6 + 2] = 0.5;
	va[numPoints * 6 + 3] = 0.5;
	va[numPoints * 6 + 4] = 0.5;
	va[numPoints * 6 + 5] = 1.0;
	GLuint* ea = new GLuint[numPoints * 3];
	for (uint32_t i = 0; i < numPoints; ++i) {
		ea[i * 3] = i;
		ea[i * 3 + 1] = i + 1;
		ea[i * 3 + 2] = numPoints;
	}
	ea[(numPoints - 1) * 3 + 1] = 0;
	hexagon = Drawable(va, (numPoints + 1) * 6, ea, numPoints * 3);

	socky.msgCb = onReceive;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(127 << 24 | 0 << 16 | 0 << 8 | 1);
	server.sin_port = htons((uint16_t)49152);

	socky.sendMessage(&server, (uint8_t*)"\x01\0", (uint16_t)2);

	float targetDt = 1.0;

	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		socky.processMessages();

		now.refresh();
		uint64_t dif = now - prevTime;
		float dt = dif / 16666.0;
		if (dt >= targetDt) {
			prevTime = now;
			targetDt = std::max(1 - (dt - targetDt), 0.0f);

			prevSecond = second;
			second = glfwGetTime();
			++fps;
			if (second != prevSecond) {
				std::cout << "fps " << fps << "\n";
				fps = 0;
			}

			update(dt);

			hexagon.addToList(vbod, ebod);
			draw();
			vbod.clear();
			ebod.clear();
		}
	}

	cleanup();
	return 0;
}
