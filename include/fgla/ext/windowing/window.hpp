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

  inline void poll_events() { return this->impl->poll_events(); }
  inline bool is_open() { return this->impl->is_open(); }

  struct Impl {
    virtual void poll_events() = 0;
    virtual bool is_open() = 0;

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
