#pragma once

#include <cstdint>
#include <fgla/instance.hpp>
#include <fgla/util.hpp>
#include <unordered_map>

namespace fgla::backend {

/// A Universally Unique Identifier identifying a `Backend`
using BackendUUID = util::UUID<16>;

/// Represents a backend, implementing `fgla`'s constructs and translating them to hardware
struct Backend {
  /// A unique identifier for the `Backend`
  BackendUUID uuid;
  /// A name representing the `Backend`
  const char *name;
  /// Checks if the `Backend` is actually available in the current environment
  bool (*is_available)();
  /// Creates an `Instance` using this `Backend`
  Instance *(*create_instance)(const Instance::Descriptor *);
};

/// @returns a reference to the global registry of `Backend`s
inline auto &get_registry() {
  static std::unordered_map<BackendUUID, Backend, BackendUUID::Hash> registry;
  return registry;
}

/// Adds a `Backend` to the global registry
/// @param backend the `Backend` to add
inline void register_backend(const Backend &backend) {
  get_registry().insert_or_assign(backend.uuid, backend);
}

/// Adds a backend to the global registry in its constructor
/// Allows for registration during dynamic initialization
/// Care must be taken to ensure the `BackendRegisrar` isn't optimized out
struct BackendRegistrar {
  /// Registers the `Backend`
  /// @param backend the `Backend` to register
  BackendRegistrar(const Backend &backend) { register_backend(backend); }
};

/// Checks if a specified `Backend` is available
/// @param uuid The unique identifier of the `Backend` to check
/// @returns `true` if the specified `Backend` is available, else `false`
inline bool is_available(BackendUUID uuid) {
  const auto &registry = get_registry();

  auto it = registry.find(uuid);
  if (it == registry.end() || it->second.is_available == nullptr ||
      it->second.create_instance == nullptr)
    return false; // backend doesn't exist
  return it->second.is_available();
}

} // namespace fgla::backend
