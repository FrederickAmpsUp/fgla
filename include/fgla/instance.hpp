#pragma once

#include <any>
#include <fgla/adapter.hpp>
#include <fgla/error.hpp>
#include <fgla/extension.hpp>
#include <fgla/util.hpp>
#include <memory>
#include <optional>
#include <tl/expected.hpp>

namespace fgla {

namespace backend {
struct Backend;
using BackendUUID = util::UUID<16>;
} // namespace backend

class Instance {
public:
  struct Descriptor {
    std::optional<backend::BackendUUID> preferred_backend;
    std::vector<extension::ExtensionUUID> required_extensions;
    VersionTriple app_version = {1, 0, 0};
    std::string app_name = "";
  };

  static tl::expected<Instance, Error> create(const Descriptor &descriptor);

  inline tl::expected<Adapter, Error> get_adapter(const Adapter::Descriptor &descriptor) {
    return this->impl->get_adapter(descriptor);
  }
  inline const backend::Backend &get_backend() { return this->impl->get_backend(); }

  template <typename T> util::OptRef<T> get_extension() {
    extension::ExtensionUUID uuid = T::UUID;
    void *ext = this->impl->get_extension(uuid);
    if (ext) {
      return *static_cast<T *>(ext);
    } else {
      return {};
    }
  }

  struct Impl {
    virtual tl::expected<Adapter, Error> get_adapter(const Adapter::Descriptor &) = 0;
    virtual const backend::Backend &get_backend() = 0;
    virtual void *get_extension(extension::ExtensionUUID) = 0;

    virtual ~Impl() = 0;
  };

  static inline Instance from_raw(std::unique_ptr<Instance::Impl> impl) {
    Instance inst;
    inst.impl = std::move(impl);
    return inst;
  }

private:
  std::unique_ptr<Impl> impl;
};

inline Instance::Impl::~Impl() = default;

} // namespace fgla
