#pragma once

#include <fgla/instance.hpp>
#include <memory>

namespace fgla::ext::windowing {

class Surface {
public:
  struct Impl {
    virtual ~Impl() = 0;
  };

  static inline Surface from_raw(std::shared_ptr<Impl> impl) {
    Surface surface;
    surface.impl = std::move(impl);
    return surface;
  }
private:
  std::shared_ptr<Impl> impl;
};

inline Surface::Impl::~Impl() = default;

}
