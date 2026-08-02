#pragma once

#include <fgla/internal.hpp>
#include <memory>

namespace fgla {

/// Signals the completion of a submitted operation, which can be
/// waited on or used to synchronize subsequent operations
class Completion {
public:
  Completion(const Completion &other) : impl(other.impl ? other.impl->clone() : nullptr) {}

  Completion &operator=(const Completion &other) {
    if (this != &other) {
      impl = other.impl ? other.impl->clone() : nullptr;
    }
    return *this;
  }

  Completion(Completion &&) noexcept = default;
  Completion &operator=(Completion &&) noexcept = default;

  Completion() = default;

  /// The backend-defined implementation of the `Completion`'s functions
  struct Impl {
    virtual std::unique_ptr<Impl> clone() const = 0;

    virtual ~Impl() = 0;
  };

  /// Creates a `Completion` from a raw implementation
  /// This should only be used internally
  static inline Completion from_raw(std::unique_ptr<Impl> impl) {
    Completion completion;
    completion.impl = std::move(impl);
    return completion;
  }

private:
  friend struct fgla::internal::ImplAccessor;
  std::unique_ptr<Impl> impl;
};

inline Completion::Impl::~Impl() = default;

} // namespace fgla
