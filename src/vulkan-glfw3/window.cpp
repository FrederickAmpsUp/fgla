#include <fgla/window.hpp>
#include <spdlog/spdlog.h>
#include <limits>
#include <algorithm>
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

static VkSurfaceFormatKHR choose_swap_surf_format(const std::vector<VkSurfaceFormatKHR>& available) {
	for (const auto& format : available) {
		if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return format;
	}

	for (const auto& format : available) {
		if (format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return format;
	}

	return available[0]; // what formats even are there? this will work...
}

static VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR>& available) {
	for (const auto& mode : available) {
		if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
			return mode;
	}

	return VK_PRESENT_MODE_FIFO_KHR; // this one is guaranteed to be available
}

static VkExtent2D choose_swap_extent(GLFWwindow *window, const VkSurfaceCapabilitiesKHR& capabilities) {
	if (capabilities.currentExtent.width != std::numeric_limits<unsigned int>::max()) {
		return capabilities.currentExtent; // size of window
	} else { // 4,294,967,295 means we gotta figure this one out ourselves :(
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		VkExtent2D actual_extent = {
			(unsigned int)width, (unsigned int)height
		};

		actual_extent.width = std::clamp(actual_extent.width, capabilities.minImageExtent.width,
				   				      capabilities.maxImageExtent.width);
		actual_extent.height = std::clamp(actual_extent.height, capabilities.minImageExtent.height,
				    					capabilities.maxImageExtent.height);

		return actual_extent;
	}
}

result Window::NativeData::init_swapchain(VkDevice device, SwapChainSupportDetails swap_chain_support, DeviceQueueFamilyIndices indices) {
	auto surface_format = choose_swap_surf_format(swap_chain_support.formats);
	auto present_mode = choose_swap_present_mode(swap_chain_support.present_modes);
	auto extent = choose_swap_extent(this->win, swap_chain_support.capabilities);

	unsigned int image_count = swap_chain_support.capabilities.minImageCount + 1;
	if (swap_chain_support.capabilities.maxImageCount > 0 && image_count > swap_chain_support.capabilities.maxImageCount) {
		image_count = swap_chain_support.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR create_info{};
	create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	create_info.surface = this->surface;

	create_info.minImageCount = image_count;
	create_info.imageFormat = surface_format.format;
	create_info.imageColorSpace = surface_format.colorSpace;
	create_info.imageExtent = extent;
	create_info.imageArrayLayers = 1;
	create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

	unsigned int queue_family_indices[] = { indices.graphics.value(), indices.present.value() };
	if (indices.graphics != indices.present) {
		create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		create_info.queueFamilyIndexCount = 2;
		create_info.pQueueFamilyIndices = queue_family_indices;
	} else {
		create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	create_info.preTransform = swap_chain_support.capabilities.currentTransform;
	create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	create_info.presentMode = present_mode;
	create_info.clipped = VK_TRUE;

	create_info.oldSwapchain = this->swapchain;

	VkResult res = vkCreateSwapchainKHR(device, &create_info, nullptr, &this->swapchain);
	if (res != VK_SUCCESS) {
		return cpp::fail("vkCreateSwapchainKHR failed!");
	}

	spdlog::info("fgla::Window::NativeData::init_swapchain [internal]: Created vkSwapchainKHR with size {}x{}", extent.width, extent.height);

	this->device = device;

	return {};
}

Window::~Window() {
	vkDestroySwapchainKHR(this->data->device, this->data->swapchain, nullptr);
	vkDestroySurfaceKHR(this->data->instance, this->data->surface, nullptr);
	glfwDestroyWindow(this->data->win);
	delete this->data;
	spdlog::info("fgla::Window::~Window: Cleaned up.");
}
}
