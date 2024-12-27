#pragma once

#include <fgla/fgla.hpp>
#include <optional>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <set>

void initialize();

namespace fgla {

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> present_modes;
};

struct DeviceQueueFamilyIndices {
	std::optional<unsigned int> graphics;
	std::optional<unsigned int> present;
	
	bool is_complete() const {
		return this->graphics.has_value() && this->present.has_value();
	}

	std::set<unsigned int> unique_families() {
		if (!this->is_complete()) return {};
		return { this->graphics.value(), this->present.value() };
	}
};

struct Window::NativeData {
	GLFWwindow *win;
	VkInstance instance;
	VkSurfaceKHR surface = VK_NULL_HANDLE;
	VkSwapchainKHR swapchain = VK_NULL_HANDLE;
	VkDevice device = VK_NULL_HANDLE;

	result init_swapchain(VkDevice device, SwapChainSupportDetails swap_chain_support, DeviceQueueFamilyIndices indices);
};

struct Instance::NativeData {
	VkInstance instance;
	bool ok;
};

struct Device::NativeData {
	VkPhysicalDevice physical_device;
	VkDevice device = VK_NULL_HANDLE;
	VkQueue graphics_queue = VK_NULL_HANDLE;
	VkQueue present_queue = VK_NULL_HANDLE;
	std::string name;
	const Window *window;
};
}
