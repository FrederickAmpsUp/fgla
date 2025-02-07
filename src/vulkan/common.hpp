#pragma once

#include <fgla/fgla.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <optional>

#include <unordered_set>

namespace fgla {

struct QueueFamilyIndices {
	std::unordered_set<uint32_t> graphics;
	std::unordered_set<uint32_t> compute;
	std::unordered_set<uint32_t> transfer;
	std::unordered_set<uint32_t> sparse_binding;
	std::unordered_set<uint32_t> present;

	bool is_complete() {
		return !graphics.empty() && !compute.empty() && !transfer.empty() && !sparse_binding.empty();
	}

	bool is_present_complete() {
		return is_complete() && !present.empty();
	}

	static QueueFamilyIndices find(VkPhysicalDevice device) {
		QueueFamilyIndices indices;

		uint32_t n_queue_families = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &n_queue_families, nullptr);

		std::vector<VkQueueFamilyProperties> queue_families(n_queue_families);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &n_queue_families, queue_families.data());

		uint32_t i = 0;
		for (const auto& family : queue_families) {
			if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphics.insert(i);
			}

			if (family.queueFlags & VK_QUEUE_COMPUTE_BIT) {
				indices.compute.insert(i);
			}

			if (family.queueFlags & VK_QUEUE_TRANSFER_BIT) {
				indices.transfer.insert(i);
			}

			if (family.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) {
				indices.sparse_binding.insert(i);
			}

			// TODO: present family

			++i;
		}

		return indices;
	}
};

struct Device::NativeData {
	VkPhysicalDevice physical_device = VK_NULL_HANDLE;
	VkDevice device = VK_NULL_HANDLE;
};

struct Instance::NativeData {
	VkInstance instance = VK_NULL_HANDLE;

	std::vector<VkPhysicalDevice> physical_devices;
};

struct Window::NativeData {
	GLFWwindow *window = nullptr;

	static uint32_t n_open_windows;
};

} // namespace fgla
