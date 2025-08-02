#pragma once

#include <fgla/adapter.hpp>
#include <fgla/error.hpp>
#include <fgla/extension.hpp>
#include <fgla/util.hpp>
#include <memory>
#include <optional>
#include <tl/expected.hpp>
#include <fgla/internal.hpp>

namespace fgla {

namespace backend {
struct Backend;
using BackendUUID = util::UUID<16>;
} // namespace backend

/// Represents an instance, which is used to acquire an `Adapter`
class Instance {
public:
  /// Represents the settings used to create an `Instance`
  struct Descriptor {
    /// The `BackendUUID` of a backend to use over any others available
    std::optional<backend::BackendUUID> preferred_backend;
    /// A list of `ExtensionUUID`s for required extensions
    /// If a backend does not support a required extension, it will not be used
    std::vector<extension::ExtensionUUID> required_extensions;
    /// The version of the application using `fgla`
    VersionTriple app_version = {1, 0, 0};
    /// The name of the application using `fgla`
    std::string app_name = "";
  };

  /// Creates an `Instance`
  /// @param descriptor The `Descriptor` with the `Instance` creation settings
  /// @returns The created `Instance`, or an `Error` with failure information
  static tl::expected<Instance, Error> create(const Descriptor &descriptor);

  /// Acquires an `Adapter`
  /// @param descriptor The `Adapter::Descriptor` with the `Adapter`'s properties
  /// @returns The `Adapter`, or an `Error` with failure information
  inline tl::expected<Adapter, Error> get_adapter(const Adapter::Descriptor &descriptor) {
    return this->impl->get_adapter(descriptor);
  }

  /// @returns the `Backend` this `Instance` is using
  inline const backend::Backend &get_backend() { return this->impl->get_backend(); }

  /// Retrieves an extension from its type
  /// @tparam T The extension to retrieve
  /// @returns The extension, or an empty `util::OptRef` if not supported
  template <typename T> util::OptRef<T> get_extension() {
    extension::ExtensionUUID uuid = T::UUID;
    void *ext = this->impl->get_extension(uuid);
    if (ext) {
      return *static_cast<T *>(ext);
    } else {
      return {};
    }
  }

  /// The backend-defined implementation of the `Instance`'s functions
  struct Impl {
    virtual tl::expected<Adapter, Error> get_adapter(const Adapter::Descriptor &) = 0;
    virtual const backend::Backend &get_backend() = 0;
    virtual void *get_extension(extension::ExtensionUUID) = 0;

    virtual ~Impl() = 0;
  };

  /// Creates an `Instance` from a raw imlementation
  /// This should only be used internally
  static inline Instance from_raw(std::unique_ptr<Instance::Impl> impl) {
    Instance inst;
    inst.impl = std::move(impl);
    return inst;
  }

private:
  std::unique_ptr<Impl> impl;
  friend struct fgla::internal::ImplAccessor;
};

inline Instance::Impl::~Impl() = default;

} // namespace fgla
