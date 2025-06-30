#pragma once

#include <cassert>
#include <cstdint>
#include <vector>

namespace fgla {
struct VersionTriple {
  int major, minor, patch;
};
} // namespace fgla

namespace fgla::util {
constexpr uint8_t parse_hex(char c) {
  return (c >= '0' && c <= '9')   ? c - '0'
         : (c >= 'a' && c <= 'f') ? c - 'a' + 10
         : (c >= 'A' && c <= 'F') ? c - 'A' + 10
                                  : throw "Invalid hex digit in UUID";
}

struct PairHash {
  template <typename T, typename U> std::size_t operator()(const std::pair<T, U> &p) const {
    return std::hash<T>()(p.first) ^ (std::hash<U>()(p.second) << 1);
  }
};

template <unsigned int size> struct UUID {
  uint8_t bytes[size];

  constexpr bool operator==(const UUID<size> &other) const {
    for (int i = 0; i < size; ++i)
      if (bytes[i] != other.bytes[i]) return false;
    return true;
  }

  constexpr bool operator!=(const UUID<size> &other) const { return !(*this == other); }

  struct Hash {
    std::size_t operator()(const UUID<size> &id) const {
      std::size_t h = 0;
      for (int i = 0; i < size; ++i)
        h ^= std::size_t(id.bytes[i]) << ((i % sizeof(std::size_t)) * 8);
      return h;
    }
  };

  static constexpr UUID<size> parse_ctr(const char *str) {
    static_assert(size == 16, "UUID::parse_ctr only implemented for UUID<16>");

    UUID<16> uuid = {};
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
};

template <typename T> class OptRef {
public:
  OptRef() : ptr(nullptr) {}
  OptRef(T &ref) : ptr(&ref) {}

  bool has_value() const { return this->ptr != nullptr; }
  explicit operator bool() const { return this->has_value(); }

  T &value() const {
    assert(this->ptr && "OptRef::value() called on empty OptRef");
    return *this->ptr;
  };

  T *operator->() const {
    assert(this->ptr && "OptRef::operator-> called on empty OptRef");
    return this->ptr;
  }

  T &operator*() const {
    assert(this->ptr && "OptRef::operator* called on empty OptRef");
    return *this->ptr;
  }

private:
  T *ptr;
};

} // namespace fgla::util
