#pragma once

#include <fgla/fgla.hpp>
#include <optional>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

void initialize();

namespace fgla {

struct Window::NativeData {
	GLFWwindow *win;
	VkInstance instance;
	VkSurfaceKHR surface = VK_NULL_HANDLE;
};

struct Instance::NativeData {
	VkInstance instance;
	bool ok;
};

struct DeviceQueueFamilyIndices {
	std::optional<unsigned int> graphics;
	std::optional<unsigned int> present;
	
	bool is_complete() const {
		return this->graphics.has_value() && this->present.has_value();
	}
};

struct Device::NativeData {
	VkPhysicalDevice physical_device;
	VkDevice device = VK_NULL_HANDLE;
	VkQueue graphics_queue = VK_NULL_HANDLE;
	std::string name;
};
}
