#pragma once

namespace fgla::internal {

/// Allows access to the raw polymorphic implementation of FGLA classes
struct ImplAccessor {

  /// Get a pointer to an FGLA class' implementation
  /// @tparam T The type of FGLA class to unwrap
  /// @param t The object to unwrap
  /// @returns A pointer to `t`'s polymporphic implementation
  template <typename T> static inline typename T::Impl *get_impl(T &t) { return t.impl.get(); }
};

} // namespace fgla::internal
