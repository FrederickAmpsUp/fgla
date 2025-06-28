#include <fgla/backends/vulkan/adapter.hpp>
#include <fgla/backends/vulkan/device.hpp>
#include <fgla/backends/vulkan/queue.hpp>
#include <spdlog/spdlog.h>
#include <vulkan/vulkan_core.h>

namespace fgla::backends::vulkan {

static int score_physical_device(VkPhysicalDevice device, const Adapter::Descriptor& descriptor) {
	VkPhysicalDeviceProperties props;
	vkGetPhysicalDeviceProperties(device, &props);

	VkPhysicalDeviceFeatures features;
	vkGetPhysicalDeviceFeatures(device, &features);

	QueueFamilyIndices indices = QueueFamilyIndices::find(device);

	if (!indices.graphics_family.has_value()) {
		return 0; // we need a device that can draw stuff, probably
	}

	int score = 0;

	// Prefer discrete GPUs
	if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
		score += 10000;
	} else if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
		score += 1000;
	}

	// Add score based on VRAM (in MB)
	VkPhysicalDeviceMemoryProperties memProps;
	vkGetPhysicalDeviceMemoryProperties(device, &memProps);

	VkDeviceSize totalVRAM = 0;
	for (uint32_t i = 0; i < memProps.memoryHeapCount; i++) {
		if (memProps.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
			totalVRAM += memProps.memoryHeaps[i].size;
		}
	}
	score += static_cast<int>(totalVRAM / (4 * 1024 * 1024));

	// need more scoring later
	return score;
}

QueueFamilyIndices QueueFamilyIndices::find(VkPhysicalDevice device) {
	uint32_t n_queue_families = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &n_queue_families, nullptr);

	std::vector<VkQueueFamilyProperties> queue_families(n_queue_families);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &n_queue_families, queue_families.data());

	QueueFamilyIndices indices;

	uint32_t i = 0;
	for (const auto& queue_family : queue_families) {
		if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphics_family = i;
		}

		++i;
	}

	return indices;
}

AdapterImpl::AdapterImpl(const InstanceImpl& instance_impl, const Adapter::Descriptor& descriptor) {
	static auto logger = spdlog::get("fgla::backends::vulkan");

	VkInstance instance = instance_impl.get_instance();

	uint32_t n_devices = 0;
	vkEnumeratePhysicalDevices(instance, &n_devices, nullptr);

	if (n_devices == 0) {
		logger->error("No physical devices available!");
		this->physical_device = VK_NULL_HANDLE;
		return;
	}

	std::vector<VkPhysicalDevice> devices(n_devices);
	vkEnumeratePhysicalDevices(instance, &n_devices, devices.data());

	VkPhysicalDevice selected_device = VK_NULL_HANDLE;
	std::string selected_device_name;
	int best_score = INT_MIN;

	logger->info("{} physical devices available:", n_devices);
	
	for (VkPhysicalDevice candidate : devices) {
		int candidate_score = score_physical_device(candidate, descriptor);

		VkPhysicalDeviceProperties candidate_props;
		vkGetPhysicalDeviceProperties(candidate, &candidate_props);

		std::string vendor_name = fmt::format("Unknown, 0x{:4X}", candidate_props.vendorID);
		
		switch (candidate_props.vendorID) {
			case 0x10DE: vendor_name = "NVIDIA"; break;
			case 0x1002: vendor_name = "AMD"; break;
			case 0x8086: vendor_name = "Intel"; break;
		}

		logger->info(" - \"{}\", vendor \"{}\", score {}", candidate_props.deviceName, vendor_name, candidate_score);
		
		if (candidate_score > best_score) {
			selected_device = candidate;
			selected_device_name = std::string(candidate_props.deviceName);
			best_score = candidate_score;
		}
	}

	if (best_score <= 0) {
		logger->error("No suitable physical devices available!");
		this->physical_device = VK_NULL_HANDLE;
		return;
	}

	logger->info("Selected device \"{}\".", selected_device_name);

	this->physical_device = selected_device;
}

bool AdapterImpl::is_ok() const {
	return this->physical_device != VK_NULL_HANDLE;
}

tl::expected<Device, Error> AdapterImpl::create_device(const Device::Descriptor& descriptor, std::initializer_list<Queue::Request> queue_requests) {
	QueueAllocator queue_allocator = QueueAllocator(queue_requests, this->physical_device);

	std::unique_ptr<DeviceImpl> impl = std::make_unique<DeviceImpl>(*this, descriptor, queue_allocator);
	if (!impl->is_ok()) return tl::make_unexpected(Error(ErrorCode::CREATE_DEVICE_FAILED));
	return std::move(Device::from_raw(std::move(impl)));
}

Adapter::Info AdapterImpl::get_info() const {
	if (!this->is_ok()) {
		return Adapter::Info {
			.device_name = "none"
		};
	}
	Adapter::Info info;

	VkPhysicalDeviceProperties device_props;
	vkGetPhysicalDeviceProperties(this->physical_device, &device_props);

	info.device_name = std::string(device_props.deviceName);

	return info;
}

}
