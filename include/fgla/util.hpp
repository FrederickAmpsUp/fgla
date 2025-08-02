#pragma once

#include <cassert>
#include <cstdint>
#include <vector>
#include <algorithm>

namespace fgla {

/// A 3-number version identifier
struct VersionTriple {
  int major, minor, patch;
};
} // namespace fgla

namespace fgla::util {
/// Converts an ASCII character to a hex digit that character represents
/// Example: '5' => 5, 'C' => 12
/// @param c The character to convert
/// @returns The hex digit corresponding to `c`
constexpr uint8_t parse_hex(char c) {
  return (c >= '0' && c <= '9')   ? c - '0'
         : (c >= 'a' && c <= 'f') ? c - 'a' + 10
         : (c >= 'A' && c <= 'F') ? c - 'A' + 10
                                  : throw "Invalid hex digit in UUID";
}

/// A hash for a `std::pair`, which may be used in STL containers
struct PairHash {
  template <typename T, typename U> std::size_t operator()(const std::pair<T, U> &p) const {
    return std::hash<T>()(p.first) ^ (std::hash<U>()(p.second) << 1);
  }
};

/// A Univerally Unique Identifier, with a given size in bytes
template <unsigned int size> struct UUID {
  /// The `UUID`'s data
  uint8_t bytes[size];

  /// Check if 2 `UUID`s are the same
  /// @param other The `UUID` to compare with `this`
  /// @returns `true` if `this` and `other` are equal, else false
  constexpr bool operator==(const UUID<size> &other) const {
    for (int i = 0; i < size; ++i)
      if (bytes[i] != other.bytes[i]) return false;
    return true;
  }

  /// Check if 2 `UUID`s are different
  constexpr bool operator!=(const UUID<size> &other) const { return !(*this == other); }

  /// A hash for a `UUID`, which may be used in STL containers
  struct Hash {
    std::size_t operator()(const UUID<size> &id) const {
      std::size_t h = 0;
      for (int i = 0; i < size; ++i)
        h ^= std::size_t(id.bytes[i]) << ((i % sizeof(std::size_t)) * 8);
      return h;
    }
  };

  /// Parse a `UUID` from a canonical text representation
  /// 00000000-0000-0000-0000-000000000000 format
  ///
  /// @param str The string to parse from
  /// @returns The parsed `UUID`
  ///
  /// Note: `size` must be 16
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

/// An alternative to `std::optional` for references
template <typename T> class OptRef {
public:
  /// Creates an empty `OptRef`
  OptRef() : ptr(nullptr) {}
  /// Creates an `OptRef` referencing a value
  /// @param ref The value to reference
  OptRef(T &ref) : ptr(&ref) {}

  /// Check if this `OptRef` is referencing any value
  /// @returns `true` if this `OptRef` contains a reference, else `false`
  bool has_value() const { return this->ptr != nullptr; }
  /// Equivalent to `has_value`
  explicit operator bool() const { return this->has_value(); }

  /// Retrieve the reference from this `OptRef`
  /// Panics in debug mode if this `OptRef` is empty
  T &value() const {
    assert(this->ptr && "OptRef::value() called on empty OptRef");
    return *this->ptr;
  };

  /// Retrieve the reference from this `OptRef`
  /// Panics in debug mode if this `OptRef` is empty
  T *operator->() const {
    assert(this->ptr && "OptRef::operator-> called on empty OptRef");
    return this->ptr;
  }

  /// Retrieve the reference from this `OptRef`
  /// Panics in debug mode if this `OptRef` is empty
  T &operator*() const {
    assert(this->ptr && "OptRef::operator* called on empty OptRef");
    return *this->ptr;
  }

private:
  T *ptr;
};

/// A list that may be filtered using functions
template<typename T>
class FilterableList {
public:
  /// Creates a `FilterableList` from a vector of items
  /// @param items The items to move into this `FilterableList`
  FilterableList(std::vector<T> items) : items(std::move(items)) {}

  /// Applies a filter to the `FilterableList`
  /// @param pred The predicate function to filter by
  /// @returns A `FilterableList` containing only the elements from `this` which `pred` accepts
  ///
  /// @note `this` is consumed by this function
  template<typename Pred>
  FilterableList filter_move(Pred &&pred) && {
    std::vector<T> out;
    for (auto &item : this->items) {
        if (pred(item)) {
            out.push_back(std::move(item));
        }
    }
    return FilterableList(std::move(out));
  }

  const std::vector<T>& data() const { return this->items; }
private:
  std::vector<T> items;
};

} // namespace fgla::util
