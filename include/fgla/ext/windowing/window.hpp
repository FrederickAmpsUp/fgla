#pragma once

#include <cstdint>
#include <memory>

namespace fgla::ext::windowing {

class Window {
public:
  struct Descriptor {
    uint32_t width, height;
    const char *name;
  };

  struct Impl {
    virtual ~Impl() = 0;
  };

  static inline Window from_raw(std::unique_ptr<Impl> impl) {
    Window window;
    window.impl = std::move(impl);
    return window;
  }

private:
  std::unique_ptr<Impl> impl;
};

inline Window::Impl::~Impl() = default;
} // namespace fgla::ext::windowing
