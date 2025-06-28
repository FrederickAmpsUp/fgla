#pragma once

#include <fgla/adapter.hpp>

namespace fgla::backends::opengl {

struct AdapterImpl : public fgla::Adapter::Impl {
  virtual ~AdapterImpl() override = default;
};

} // namespace fgla::backends::opengl
