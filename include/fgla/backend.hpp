#pragma once

#include <cstdint>
#include <fgla/instance.hpp>
#include <fgla/util.hpp>
#include <unordered_map>

namespace fgla::backend {

struct BackendUUID {
  uint8_t bytes[16];

  constexpr bool operator==(const BackendUUID &other) const {
    for (int i = 0; i < 16; ++i)
      if (bytes[i] != other.bytes[i]) return false;
    return true;
  }

  constexpr bool operator!=(const BackendUUID &other) const { return !(*this == other); }

  struct Hash {
    std::size_t operator()(const BackendUUID &id) const {
      std::size_t h = 0;
      for (int i = 0; i < 16; ++i)
        h ^= std::size_t(id.bytes[i]) << ((i % sizeof(std::size_t)) * 8);
      return h;
    }
  };
};

constexpr BackendUUID parse_backend_uuid(const char *str) {
  BackendUUID uuid = {};
  int byte_index = 0;

  for (int i = 0; str[i] && byte_index < 16;) {
    if (str[i] == '-') {
      ++i;
      continue;
    }

    uint8_t hi = util::parse_hex(str[i]);
    uint8_t lo = util::parse_hex(str[i + 1]);
    uuid.bytes[byte_index++] = (hi << 4) | lo;
    i += 2;
  }

  if (byte_index != 16) throw "UUID must be exactly 16 bytes";

  return uuid;
}

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
