#include <fgla/backends/vulkan/backend.hpp>
#include <fgla/backends/vulkan/instance.hpp>
#include <fgla/util.hpp>

namespace fgla::backends::vulkan {

extern "C" bool fgla_backends_vulkan_is_available() {
	return true;
}

extern "C" Instance *fgla_backends_vulkan_create_instance(const Instance::Descriptor *descriptor) {
	std::unique_ptr<Instance::Impl> impl = std::make_unique<InstanceImpl>(*descriptor);
	auto instance = Instance::from_raw(std::move(impl));
	return new Instance(std::move(instance));
}

static const fgla::backend::BackendRegistrar registrar = { fgla::backend::Backend {
	.uuid = UUID,
	.name = "Vulkan",
	.is_available = &fgla_backends_vulkan_is_available,
	.create_instance = &fgla_backends_vulkan_create_instance
}};
}
