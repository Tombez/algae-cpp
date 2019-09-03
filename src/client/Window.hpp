#pragma once

#include <functional>
#include <vector>
#include <utility>

class Window {
public:
	GLFWwindow* gwin;
	int width;
	int height;
	int posx;
	int posy;
	bool isFullscreen;
	bool hasResized;
	int windowedWidth;
	int windowedHeight;
	int windowedX;
	int windowedY;
	std::function<void(Window&, double, double)> cursorCallback;
	std::function<void(Window&, int, int, int, int)> keyCallback;
	Window() : gwin(nullptr) {}
	Window(int w, int h, const char* title, GLFWmonitor *monitor,
		GLFWwindow *share) : width(w), height(h), posx(0), posy(0),
		isFullscreen(false), hasResized(false),
		windowedWidth(w), windowedHeight(h), windowedX(0), windowedY(0),
		cursorCallback(nullptr)
	{
		// namespace ph = std::placeholders;
		gwin = glfwCreateWindow(w, h, title, monitor, share);

		if (gwin == nullptr) {
			glfwTerminate();
			throw std::runtime_error("error initializing window");
		}
		// std::printf("a\n");
		// glfwMakeContextCurrent(gwin);
		std::printf("b %p\n", gwin);
		glfwSetWindowUserPointer(gwin, this);

		glfwSetWindowSizeCallback(gwin, &Window::onSizeChange);
		glfwSetWindowPosCallback(gwin, &Window::onPosChange);
		glfwSetCursorPosCallback(gwin, &Window::onCursorMove);
		glfwSetKeyCallback(gwin, &Window::onKeyEvent);
	}
	Window(const Window&) = delete;
	Window(Window& b) {
		gwin = b.gwin;
		b.gwin = nullptr;
		width = b.width;
		height = b.height;
		posx = b.posx;
		posy = b.posy;
		isFullscreen = b.isFullscreen;
		hasResized = b.hasResized;
		windowedX = b.windowedX;
		windowedY = b.windowedY;
		windowedWidth = b.windowedWidth;
		windowedHeight = b.windowedHeight;
		cursorCallback = b.cursorCallback;
		keyCallback = b.keyCallback;
	}
	~Window() {
		glfwDestroyWindow(gwin);
	}
	static void onSizeChange(GLFWwindow* win, int w, int h) {
		Window& self = *static_cast<Window*>(glfwGetWindowUserPointer(win));

		self.width = w;
		self.height = h;
		self.hasResized = true;
	}
	static void onPosChange(GLFWwindow* win, int x, int y) {
		Window& self = *static_cast<Window*>(glfwGetWindowUserPointer(win));

		self.posx = x;
		self.posy = y;
	}
	static void onCursorMove(GLFWwindow* win, double x, double y) {
		Window& self = *static_cast<Window*>(glfwGetWindowUserPointer(win));
		if (self.cursorCallback) {
			self.cursorCallback(self, x, y);
		}
	}
	static void onKeyEvent(GLFWwindow* win, int key, int scancode, int action, int mods) {
		Window& self = *static_cast<Window*>(glfwGetWindowUserPointer(win));
		if (self.keyCallback) {
			self.keyCallback(self, key, scancode, action, mods);
		}
	}
	void toggleFullscreen() {
		this->setFullscreen(!isFullscreen);
	}
	void setFullscreen(bool full) {
		if (full) {
			windowedWidth = width;
			windowedHeight = height;
			windowedX = posx;
			windowedY = posy;
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
			glfwSetWindowMonitor(gwin, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
		} else {
			glfwSetWindowMonitor(gwin, nullptr, windowedX, windowedY,
				windowedWidth, windowedHeight, 0);
		}
		isFullscreen = full;
	}
};
