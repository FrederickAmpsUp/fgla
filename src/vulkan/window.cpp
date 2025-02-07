#include "common.hpp"

namespace fgla {

cpp::result<Window, Error> Window::create(const Window::Desc& descriptor) {
	Window window;

	if (!glfwInit()) {
		return cpp::fail(Error {"Failed to initialize GLFW"});
	}

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	window.data->window = glfwCreateWindow(descriptor.width, descriptor.height, descriptor.title.data(), nullptr, nullptr);

	if (!window.data->window) {
		return cpp::fail(Error {"Failed to create GLFW window"});
	}

	NativeData::n_open_windows++;

	return window;
}

Window::Window(Window&& other) noexcept : data(std::move(other.data)) {
}

Window::Window() {
	this->data = std::make_unique<NativeData>();
}

Window::~Window() {
	if (!this->data) return;
	if (this->data->window) {
		glfwDestroyWindow(this->data->window);
	
		if (NativeData::n_open_windows-- == 0) {
			glfwTerminate();
		}
	}
}

uint32_t Window::NativeData::n_open_windows = 0;
}
