#include "common.hpp"
#include <algorithm>

namespace fgla {

cpp::result<Instance, Error> Instance::create(const Instance::Desc& descriptor) {
	Instance instance;

	VkApplicationInfo app_info = {};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = descriptor.app_name.data();
	app_info.applicationVersion = VK_MAKE_VERSION(descriptor.app_version.major, descriptor.app_version.minor, descriptor.app_version.patch);

	app_info.pEngineName = "fgla";
	app_info.engineVersion = VK_MAKE_VERSION(0, 0, 1);

	app_info.apiVersion = VK_API_VERSION_1_3;

	VkInstanceCreateInfo inst_info = {};
	inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	inst_info.pApplicationInfo = &app_info;

	std::vector<const char *> required_extension_names = { };

		// if we need window support, we gotta enable the required extensions
	if (descriptor.window != nullptr) {
		uint32_t n_glfw_exts = 0;
		const char **glfw_exts = glfwGetRequiredInstanceExtensions(&n_glfw_exts);
	
		required_extension_names.insert(required_extension_names.end(), glfw_exts, glfw_exts + n_glfw_exts);
	}

	inst_info.enabledExtensionCount = required_extension_names.size();
	inst_info.ppEnabledExtensionNames = required_extension_names.data();

	VkResult res = vkCreateInstance(&inst_info, nullptr, &instance.data->instance);
	if (res != VK_SUCCESS) {
		return cpp::fail(Error {"Failed to create Vulkan instance"});
	}

	uint32_t n_physical_devices = 0;
	vkEnumeratePhysicalDevices(instance.data->instance, &n_physical_devices, nullptr);

	if (n_physical_devices > 0) {
		instance.data->physical_devices.resize(n_physical_devices);
		vkEnumeratePhysicalDevices(instance.data->instance, &n_physical_devices, instance.data->physical_devices.data());
	}

	return instance;
}

static int32_t score_device(VkPhysicalDevice dev) {
	QueueFamilyIndices indices = QueueFamilyIndices::find(dev);

	if (indices.graphics.empty()) return -1;

	// TODO: real scoring

	return 1;
}

cpp::result<Device, Error> Instance::get_device(const Device::Desc& descriptor) {
	const auto& devices = this->data->physical_devices;

	std::vector<VkPhysicalDevice> devices_copy = devices;

}

Instance::Instance(Instance&& other) noexcept : data(std::move(other.data)) {
}

Instance::Instance() {
	this->data = std::make_unique<NativeData>();
}

Instance::~Instance() {
	if (!this->data) return;
	if (this->data->instance != VK_NULL_HANDLE)
		vkDestroyInstance(this->data->instance, nullptr);
}
} // namespace fgla
