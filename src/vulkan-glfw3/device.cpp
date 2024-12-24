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
			dev, std::string(props.deviceName)
		}));
	}

	return devices;
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

	score += get_device_vram(this->data->physical_device) / (128 * 1024 * 1024);

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
	delete this->data;
	spdlog::info("fgla::Device::~Device: Cleaned up.");
}
}
