#pragma once

#include <fgla/instance.hpp>

namespace fgla::backends::opengl {

struct InstanceImpl : public fgla::Instance::Impl {
  virtual tl::expected<Adapter, Error> get_adapter(const Adapter::Descriptor &descriptor) override;
  virtual const backend::Backend &get_backend() override;

  virtual ~InstanceImpl() override = default;
};

} // namespace fgla::backends::opengl
