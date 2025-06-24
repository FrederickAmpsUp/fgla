#include <fgla/backends/vulkan/backend.hpp>
#include <fgla/backends/vulkan/instance.hpp>
#include <fgla/util.hpp>

namespace fgla::backends::vulkan {

extern "C" bool fgla_backends_vulkan_is_available() {
	return false; // Not implemented
}

extern "C" Instance *fgla_backends_vulkan_create_instance() {
	std::unique_ptr<Instance::Impl> impl = std::make_unique<InstanceImpl>();
	return new Instance(std::move(Instance::from_raw(std::move(impl))));
}

static const fgla::backend::BackendRegistrar registrar = { fgla::backend::Backend {
	.uuid = UUID,
	.name = "Vulkan",
	.is_available = &fgla_backends_vulkan_is_available,
	.create_instance = &fgla_backends_vulkan_create_instance
}};
}
