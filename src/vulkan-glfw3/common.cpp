#include "common.hpp"
#include <spdlog/spdlog.h>

void initialize() {
	static bool ready = false;
	if (ready) return;
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	spdlog::info("[internal] initialize: GLFW initialized");
	ready = true;
}
