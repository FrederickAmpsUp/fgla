#pragma once

#include <fgla/instance.hpp>
#include <vulkan/vulkan.h>

namespace fgla::backends::vulkan {

struct InstanceImpl : public Instance::Impl {
	InstanceImpl(const Instance::Descriptor& desc);
	bool is_ok() const;

	inline VkInstance get_instance() const { return this->instance; }

	virtual tl::expected<Adapter, Error> get_adapter(const Adapter::Descriptor& descriptor) override;
	virtual const backend::Backend& get_backend() override;

	virtual ~InstanceImpl() override;
private:
	VkInstance instance;
};

}
