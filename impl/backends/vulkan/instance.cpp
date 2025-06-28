#include "tl/expected.hpp"
#include <fgla/backends/vulkan/instance.hpp>
#include <fgla/backends/vulkan/backend.hpp>
#include <fgla/backends/vulkan/adapter.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace fgla::backends::vulkan {

InstanceImpl::InstanceImpl(const Instance::Descriptor& descriptor) {
	static auto logger = spdlog::stdout_color_mt("fgla::backends::vulkan");
	logger->set_level(spdlog::level::debug); // TODO: modify this

	VkApplicationInfo app_info = {};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

	app_info.applicationVersion = VK_MAKE_VERSION(descriptor.app_version.major, descriptor.app_version.minor, descriptor.app_version.patch);
	app_info.pApplicationName = descriptor.app_name.c_str();

	app_info.pEngineName = "FGLA";
	app_info.engineVersion = VK_MAKE_VERSION(0, 0, 1);

	app_info.apiVersion = VK_API_VERSION_1_3;

	VkInstanceCreateInfo instance_info = {};
	instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

	instance_info.pApplicationInfo = &app_info;

	instance_info.enabledLayerCount = 0;
	instance_info.enabledExtensionCount = 0; // TODO: window system extensions when we get there
	
	VkResult result = vkCreateInstance(&instance_info, nullptr, &this->instance);
	if (result != VK_SUCCESS) {
		logger->error("Vulkan instance creation failed, error code {:X}", (unsigned int)result);
		this->instance = VK_NULL_HANDLE;
		return;
	}

	logger->info("Vulkan instance created.");
}
bool InstanceImpl::is_ok() const {
	return this->instance != VK_NULL_HANDLE;
}

tl::expected<Adapter, Error> InstanceImpl::get_adapter(const Adapter::Descriptor& descriptor) {
	std::unique_ptr<AdapterImpl> impl = std::make_unique<AdapterImpl>(*this, descriptor); // may have to change this
	if (!impl->is_ok()) return tl::make_unexpected(Error(ErrorCode::GET_ADAPTER_FAILED));
	return std::move(Adapter::from_raw(std::move(impl)));
}

const backend::Backend& InstanceImpl::get_backend() {
	return fgla::backend::get_registry().at(UUID);
}

InstanceImpl::~InstanceImpl() {
	vkDestroyInstance(this->instance, nullptr);
}

}
