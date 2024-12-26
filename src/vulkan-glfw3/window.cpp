#include <fgla/window.hpp>
#include <spdlog/spdlog.h>
#include "common.hpp"

namespace fgla {

Window::Window(const fgla::Instance& instance, unsigned int width, unsigned int height, const std::string& title) {
	initialize();
	this->data = new fgla::Window::NativeData {
		.win = glfwCreateWindow(width, height, title.c_str(), NULL, NULL),
		.instance = instance.data->instance
	};
	if (!this->data->win) {
		spdlog::error("fgla::Window::Window: GLFW window creation failed!");
	} else {
		spdlog::info("fgla::Window::Window: Created window with GLFW.");
	
		VkResult res = glfwCreateWindowSurface(instance.data->instance, this->data->win, nullptr, &this->data->surface);

		if (res != VK_SUCCESS) {
			spdlog::error("fgla::Window::Window: Failed to create window surface!");
		} else {
			spdlog::info("fgla::Window::Window: Window surface created.");
		}
	}
}

// TODO: implement funky width and height functions

bool Window::is_ok() const {
	return this->data->win != nullptr && this->data->surface != VK_NULL_HANDLE;
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
	vkDestroySurfaceKHR(this->data->instance, this->data->surface, nullptr);
	glfwDestroyWindow(this->data->win);
	delete this->data;
	spdlog::info("fgla::Window::~Window: Cleaned up.");
}
}
