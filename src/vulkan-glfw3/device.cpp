#include "common.hpp"
#include <result/result.hpp>
#include <spdlog/spdlog.h>
#include <cstdlib>

namespace fgla {

const std::vector<Device> Device::enumerate(Instance& instance) {
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
		VkPhysicalDeviceProperties props;
		vkGetPhysicalDeviceProperties(dev, &props);
		devices.push_back(Device({
			.physical_device = dev,
			.name = std::string(props.deviceName)
		}));
	}

	return devices;
}
static DeviceQueueFamilyIndices find_queue_families(VkPhysicalDevice);

result Device::initialize() {
	// this function creates the logical device
	DeviceQueueFamilyIndices indices = find_queue_families(this->data->physical_device);
	if (!indices.is_complete()) return cpp::fail("Attempted to initialize an unsuitable device!");

	VkDeviceQueueCreateInfo queue_create_info{};
	queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queue_create_info.queueFamilyIndex = indices.graphics.value();
	queue_create_info.queueCount = 1; // will be more later :)
	
	float queue_priorities[] = { 1 };
	queue_create_info.pQueuePriorities = queue_priorities;

	VkPhysicalDeviceFeatures device_features{};
	// add required features here
	
	VkDeviceCreateInfo create_info{};
	create_info.pQueueCreateInfos = &queue_create_info;
	create_info.queueCreateInfoCount = 1;

	create_info.pEnabledFeatures = &device_features;

	create_info.enabledExtensionCount = 0;
	create_info.enabledLayerCount = 0; // ignored by all modern driver implementations
	
	VkResult result = vkCreateDevice(this->data->physical_device, &create_info, nullptr, &this->data->device);
	if (result != VK_SUCCESS) {
		return cpp::fail("vkCreateDevice failed!");
	}

	spdlog::info("fgla::Device::initialize: Created VkDevice successfully!");

	vkGetDeviceQueue(this->data->device, indices.graphics.value(), 0, &this->data->graphics_queue);

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

	DeviceQueueFamilyIndices indices = find_queue_families(this->data->physical_device);
	if (!indices.is_complete()) score = -1; // we can't use the device if it doesn't support the required queue families

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
	return Device(other);
}

Device::~Device() {
	vkDestroyDevice(this->data->device, nullptr);
	delete this->data;
	spdlog::info("fgla::Device::~Device: Cleaned up.");
}
}
