#include <fgla/window.hpp>
#include <spdlog/spdlog.h>
#include "common.hpp"

namespace fgla {

Window::Window(unsigned int width, unsigned int height, const std::string& title) {
	initialize();
	this->data = new fgla::Window::NativeData {
		glfwCreateWindow(width, height, title.c_str(), NULL, NULL)
	};
	if (!this->data->win) {
		spdlog::error("fgla::Window::Window: GLFW window creation failed");
	} else {
		spdlog::info("fgla::Window::Window: Created window with GLFW");
	}
}

bool Window::is_ok() {
	return this->data->win != nullptr;
}

void Window::run() {
	if (!this->is_ok()) {
		spdlog::error("fgla::Window::run: Attempted to run a window which was improperly created.");
		return;
	}
	while (!glfwWindowShouldClose(this->data->win)) {
		glfwPollEvents();
	}
}

Window::~Window() {
	glfwDestroyWindow(this->data->win);
	delete this->data;
	spdlog::info("fgla::Window::~Window: Cleaned up.");
}
}
