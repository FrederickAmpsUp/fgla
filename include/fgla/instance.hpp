#pragma once

#include <fgla/adapter.hpp>
#include <fgla/error.hpp>
#include <fgla/extension.hpp>
#include <fgla/internal.hpp>
#include <fgla/util.hpp>
#include <memory>
#include <optional>
#include <tl/expected.hpp>

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
    return this->select_adapter(
      this->impl->get_adapter_scorer(descriptor),
      this->impl->enumerate_adapters()
    );
  }

  /// Returns a list of all available `Adapter`s
  inline std::vector<Adapter> enumerate_adapters() {
    return this->impl->enumerate_adapters();
  }

  /// Returns a function that scores an `Adapter` based on an `Adapter::Descriptor`
  inline std::function<int(const Adapter&)> get_adapter_scorer(const Adapter::Descriptor &descriptor) {
    return this->impl->get_adapter_scorer(descriptor);
  }

  /// Selects the `Adapter` with the highest score as indicated by `scorer`
  /// @param scorer The score function to score with
  /// @param adapters The list of adapters to select from
  /// @returns The highest-scoring `Adapter` from `adapters`
  /// @note The returned `Adapter` is removed from (moved out of) `adapters`
  inline tl::expected<Adapter, Error> select_adapter(const std::function<int(const Adapter&)> &scorer, std::vector<Adapter> &adapters) {
    size_t best_index;
    int best_score = -1;
  
    size_t i = 0;
    for (const Adapter &adapter : adapters) {
      int score = scorer(adapter);
      if (score > best_score) {
        best_score = score;
        best_index = i;
      }
      ++i;
    }

    if (best_score < 0) {
      return tl::make_unexpected(Error(0, "No suitable adapters found"));
    }

    Adapter best_adapter = std::move(adapters[best_index]);
    adapters.erase(adapters.begin() + best_index);
    return best_adapter;
  }

  /// Alternative to `select_adapter(..., std::vector<Adapter> &)` that takes `adapters` as an rvalue.
  /// @see `select_adapter(..., std::vector<Adapter> &)`
  inline tl::expected<Adapter, Error> select_adapter(const std::function<int(const Adapter&)> &scorer, std::vector<Adapter> &&adapters) {
    return this->select_adapter(scorer, adapters);
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
    virtual std::vector<Adapter> enumerate_adapters() = 0;
    virtual std::function<int(const Adapter&)> get_adapter_scorer(const Adapter::Descriptor &descriptor) = 0;
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
  friend struct fgla::internal::ImplAccessor;
  std::unique_ptr<Impl> impl;
};

inline Instance::Impl::~Impl() = default;

} // namespace fgla
