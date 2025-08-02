#pragma once

namespace fgla::internal {

struct ImplAccessor {
template<typename T>
  static inline typename T::Impl *get_impl(T &t) {
    return t.impl.get();
  }
};

}
