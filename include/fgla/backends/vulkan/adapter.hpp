#pragma once

#include <fgla/adapter.hpp>
#include <fgla/backends/vulkan/instance.hpp>
#include <vulkan/vulkan.h>
#include <optional>
#include <cstdint>

namespace fgla::backends::vulkan {

struct QueueFamilyIndices {
	std::optional<uint32_t> graphics_family;

	static QueueFamilyIndices find(VkPhysicalDevice device);
};

struct AdapterImpl : public fgla::Adapter::Impl {
	AdapterImpl(const InstanceImpl& instance, const Adapter::Descriptor& descriptor);
	bool is_ok() const;

	virtual Adapter::Info get_info() const override;
	virtual tl::expected<Device, Error> create_device(const Device::Descriptor& descriptor, std::initializer_list<std::reference_wrapper<const Queue::Request>> queue_requests) override;
	virtual ~AdapterImpl() override = default;

private:
	VkPhysicalDevice physical_device;
};

}
