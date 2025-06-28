#pragma once

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

} // namespace fgla::util
