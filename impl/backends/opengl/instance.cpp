#include <fgla/backends/opengl/instance.hpp>
#include <fgla/backends/opengl/backend.hpp>
#include <fgla/backends/opengl/adapter.hpp>

namespace fgla::backends::opengl {

tl::expected<Adapter, Error> InstanceImpl::get_adapter(const Adapter::Descriptor& descriptor) {
	std::unique_ptr<AdapterImpl> impl = std::make_unique<AdapterImpl>(); // may have to change this
	return std::move(Adapter::from_raw(std::move(impl)));
}

const backend::Backend& InstanceImpl::get_backend() {
	return fgla::backend::get_registry().at(UUID);
}

}
