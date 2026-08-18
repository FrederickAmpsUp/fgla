#pragma once

#include <fgla/internal.hpp>
#include <memory>

namespace fgla {

// Represents a compiled collection of shader code used to create pipelines.
class ShaderModule {
public:
  struct Descriptor {
    const std::string &module_name;
  };

  /// The backend-defined implementation of the `ShaderModule`'s functions
  struct Impl {

    virtual ~Impl() = 0;
  };

  /// Creates a `ShaderModule` from a raw implementation
  /// This should only be used internally
  static inline ShaderModule from_raw(std::unique_ptr<Impl> impl) {
    ShaderModule shader_module;
    shader_module.impl = std::move(impl);
    return shader_module;
  }

private:
  friend struct fgla::internal::ImplAccessor;
  std::unique_ptr<Impl> impl;
};

inline ShaderModule::Impl::~Impl() = default;

} // namespace fgla
