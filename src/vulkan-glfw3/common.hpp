#pragma once

#include <fgla/fgla.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

void initialize();

struct fgla::Window::NativeData {
	GLFWwindow *win;
};

struct fgla::Instance::NativeData {
	VkInstance instance;
	bool ok;
};

struct fgla::Device::NativeData {
	VkPhysicalDevice physical_device;
	std::string name;
};
