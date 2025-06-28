#include <fgla/backends/opengl/backend.hpp>
#include <fgla/backends/opengl/instance.hpp>
#include <fgla/util.hpp>

namespace fgla::backends::opengl {

extern "C" bool fgla_backends_opengl_is_available() {
  return false; // Not implemented
}

extern "C" Instance *fgla_backends_opengl_create_instance(const Instance::Descriptor *) {
  std::unique_ptr<Instance::Impl> impl = std::make_unique<InstanceImpl>();
  return new Instance(std::move(Instance::from_raw(std::move(impl))));
}

static const fgla::backend::BackendRegistrar registrar = {
    fgla::backend::Backend{.uuid = UUID,
                           .name = "OpenGL",
                           .is_available = &fgla_backends_opengl_is_available,
                           .create_instance = &fgla_backends_opengl_create_instance}};
} // namespace fgla::backends::opengl
