#pragma once

#include <cstdint>
#include <fgla/instance.hpp>
#include <fgla/util.hpp>
#include <unordered_map>

namespace fgla::backend {

using BackendUUID = util::UUID<16>;

struct Backend {
  BackendUUID uuid;
  const char *name;
  bool (*is_available)();
  Instance *(*create_instance)(const Instance::Descriptor *);
};

inline auto &get_registry() {
  static std::unordered_map<BackendUUID, Backend, BackendUUID::Hash> registry;
  return registry;
}

inline void register_backend(const Backend &backend) {
  get_registry().insert_or_assign(backend.uuid, backend);
}

struct BackendRegistrar {
  BackendRegistrar(const Backend &backend) { register_backend(backend); }
};

inline bool is_available(BackendUUID uuid) {
  const auto &registry = get_registry();

  auto it = registry.find(uuid);
  if (it == registry.end() || it->second.is_available == nullptr ||
      it->second.create_instance == nullptr)
    return false; // backend doesn't exist
  return it->second.is_available();
}

} // namespace fgla::backend
