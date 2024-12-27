#include "common.hpp"
#include <result/result.hpp>
#include <spdlog/spdlog.h>
#include <cstdlib>
#include <set>

namespace fgla {

const std::vector<const char *> device_extensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

static DeviceQueueFamilyIndices find_queue_families(VkPhysicalDevice dev) {
	DeviceQueueFamilyIndices indices;	

	uint32_t n_queue_families = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(dev, &n_queue_families, nullptr);

	if (n_queue_families == 0) return indices;

	std::vector<VkQueueFamilyProperties> queue_families(n_queue_families);
	vkGetPhysicalDeviceQueueFamilyProperties(dev, &n_queue_families, queue_families.data());

	int i = 0;
	for (const auto& family : queue_families) {
		if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphics = i;
			indices.present = i; // NOTE:
			// This is technically incorrect. The present and graphics families are NOT guaranteed 
			// to be the same, and it's not impossible for a present family to be missing completely.
			// However, most of the time, this will work, and it would be a pain to pass the window surface
			// all the way into this function. If this becomes an issue later, I'll change it.
		}

		if (indices.is_complete()) break;
		++i;
	}
	return indices;
}

static bool check_device_extension_support(VkPhysicalDevice dev) {
	unsigned int n_extensions = 0;
	vkEnumerateDeviceExtensionProperties(dev, nullptr, &n_extensions, nullptr);

	std::vector<VkExtensionProperties> available_extensions(n_extensions);
	vkEnumerateDeviceExtensionProperties(dev, nullptr, &n_extensions, available_extensions.data());

	std::set<std::string> required_extensions(device_extensions.begin(), device_extensions.end());

	for (const auto& extension : available_extensions) {
		required_extensions.erase(extension.extensionName);
	}

	return required_extensions.empty();
}


static SwapChainSupportDetails query_swap_chain_support(VkPhysicalDevice dev, VkSurfaceKHR surface) {
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(dev, surface, &details.capabilities);

	unsigned int n_formats = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(dev, surface, &n_formats, nullptr);

	if (n_formats != 0) {
		details.formats.resize(n_formats);
		vkGetPhysicalDeviceSurfaceFormatsKHR(dev, surface, &n_formats, details.formats.data());
	}

	unsigned int n_present_modes = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(dev, surface, &n_present_modes, nullptr);

	if (n_present_modes != 0) {
		details.present_modes.resize(n_present_modes);
		vkGetPhysicalDeviceSurfacePresentModesKHR(dev, surface, &n_present_modes, details.present_modes.data());
	}

	return details;
}

static bool check_device_suitability(VkPhysicalDevice dev, VkSurfaceKHR surface) {
	DeviceQueueFamilyIndices indices = find_queue_families(dev);

	bool extensions_ok = check_device_extension_support(dev);
	
	bool swap_chain_ok = false;
	if (extensions_ok) {
		SwapChainSupportDetails swap_chain_support = query_swap_chain_support(dev, surface);
		
		swap_chain_ok = !swap_chain_support.formats.empty() && !swap_chain_support.present_modes.empty();
	}

	return indices.is_complete() && extensions_ok && swap_chain_ok;
}

const std::vector<Device> Device::enumerate(const Instance& instance, const Window& window) {
	std::vector<Device> devices;

	unsigned int n_devs = 0;
	vkEnumeratePhysicalDevices(instance.data->instance, &n_devs, nullptr);

	if (n_devs == 0) {
		spdlog::warn("fgla::Device::enumerate: No physical devices available.");
		return {};
	} else {
		spdlog::info("fgla::Device::enumerate: {} physical devices available.", n_devs);
	}

	std::vector<VkPhysicalDevice> vk_devices(n_devs);
	vkEnumeratePhysicalDevices(instance.data->instance, &n_devs, vk_devices.data());

	for (auto dev : vk_devices) {
		if (!check_device_suitability(dev, window.data->surface)) continue;
		VkPhysicalDeviceProperties props;
		vkGetPhysicalDeviceProperties(dev, &props);
		devices.push_back(Device({
			.physical_device = dev,
			.name = std::string(props.deviceName),
			.window = &window
		}));
	}

	return devices;
}

result Device::initialize() {
	if (this->data->device != VK_NULL_HANDLE) {
		spdlog::warn("fgla::Device::initialize: Attempted to initialize a device which is already initialized.");
		return {};
	}
	// this function creates the logical device
	DeviceQueueFamilyIndices indices = find_queue_families(this->data->physical_device);
	if (!indices.is_complete()) return cpp::fail("Attempted to initialize an unsuitable device!");

	std::vector<VkDeviceQueueCreateInfo> queue_create_infos;

	auto unique_queue_families = indices.unique_families();
	
	float queue_priority = 1.0f;
	for (unsigned int family : unique_queue_families) {
		VkDeviceQueueCreateInfo queue_create_info{};
		queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_create_info.queueFamilyIndex = family;
		queue_create_info.queueCount = 1;
	
		queue_create_info.pQueuePriorities = &queue_priority;
		queue_create_infos.push_back(queue_create_info);
	}

	VkPhysicalDeviceFeatures device_features{};
	// add required features here
	
	VkDeviceCreateInfo create_info{};
	create_info.pQueueCreateInfos = queue_create_infos.data();
	create_info.queueCreateInfoCount = queue_create_infos.size();

	create_info.pEnabledFeatures = &device_features;

	create_info.enabledExtensionCount = device_extensions.size();
	create_info.ppEnabledExtensionNames = device_extensions.data();

	create_info.enabledLayerCount = 0; // ignored by all modern driver implementations
	
	VkResult result = vkCreateDevice(this->data->physical_device, &create_info, nullptr, &this->data->device);
	if (result != VK_SUCCESS) {
		return cpp::fail("vkCreateDevice failed!");
	}

	spdlog::info("fgla::Device::initialize: Created VkDevice successfully!");

	vkGetDeviceQueue(this->data->device, indices.graphics.value(), 0, &this->data->graphics_queue);
	vkGetDeviceQueue(this->data->device, indices.present.value(), 0, &this->data->present_queue);

	SwapChainSupportDetails swc_support = query_swap_chain_support(this->data->physical_device, this->data->window->data->surface);

	auto win_swc_res = this->data->window->data->init_swapchain(this->data->device, swc_support, indices);
	if (win_swc_res.has_error()) return win_swc_res;

	return {};
}

Device::Device(NativeData d) {
	this->data = new NativeData;
	*this->data = d;
}

const std::string& Device::name() const {
	return this->data->name;
}

static VkDeviceSize get_device_vram(VkPhysicalDevice device) {
    VkPhysicalDeviceMemoryProperties props;
    vkGetPhysicalDeviceMemoryProperties(device, &props);

    for (uint32_t i = 0; i < props.memoryHeapCount; ++i) {
        if (props.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
            return props.memoryHeaps[i].size;
        }
    }

    return 0;
}

int Device::score() const {
	if (this->data->physical_device == VK_NULL_HANDLE) return -1;
	VkPhysicalDeviceProperties props;
	VkPhysicalDeviceFeatures features;

	vkGetPhysicalDeviceProperties(this->data->physical_device, &props);
	vkGetPhysicalDeviceFeatures(this->data->physical_device, &features);

	int score = 0;

	if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) score += 1000;
	if (props.vendorID == 0x10DE) score -= 10; // we are slightly against NVIDIA
	if (props.vendorID == 0x8086) score += 10; // we love Intel for their vendor ID
	
	score += (props.limits.maxImageDimension2D - 1024) / 64;
	score += (props.limits.maxViewportDimensions[0] - 1024) / 64;

	score += get_device_vram(this->data->physical_device) / (128 * 1024 * 1024); // 10pts for every gibibyte of VRAM

	return score;
}

unsigned long long Device::available_memory() const {
	return get_device_vram(this->data->physical_device);
}

Device::Device(const Device& other) {
	this->data = new NativeData;
	*this->data = *other.data;
}

Device::Device() {
	this->data = new NativeData;
	this->data->physical_device = VK_NULL_HANDLE;
}

Device Device::operator=(const Device& other) {
	if (this == &other) return *this;
	if (this->data->device != VK_NULL_HANDLE) vkDestroyDevice(this->data->device, nullptr);
	
	*this->data = *other.data;
	return *this;
}

Device::~Device() {
	vkDestroyDevice(this->data->device, nullptr);
	delete this->data;
	spdlog::info("fgla::Device::~Device: Cleaned up.");
}
}
