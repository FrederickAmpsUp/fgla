#pragma once

#include <cstdint>
#include <memory>
#include <fgla/ext/windowing/surface.hpp>

namespace fgla::ext::windowing {

/// Represents a window, which may be used to draw to and interface with input devices
class Window {
public:
  /// Represents the settings used to create a `Window`
  struct Descriptor {
    /// The width of the window, in pixels
    uint32_t width = 800;
    /// The height of the window, in pixels
    uint32_t height = 600;
    /// The name or title of the window
    const char *name = "FGLA Window";
  };

  /// Internally runs the window's event loop
  inline void poll_events() { return this->impl->poll_events(); }

  /// Checks if the window has not been closed by the user
  /// @returns `false` if the user has closed the window, `true` otherwise
  inline bool is_open() { return this->impl->is_open(); }

  inline tl::expected<Surface, Error> create_surface(const fgla::Instance& instance) { return this->impl->create_surface(instance); }

  /// The backend-defined implementation of the `Window`'s functions
  struct Impl {
    virtual void poll_events() = 0;
    virtual bool is_open() = 0;

    virtual tl::expected<Surface, Error> create_surface(const fgla::Instance&) = 0;

    virtual ~Impl() = 0;
  };

  /// Creates a `Window` from a raw implementation
  /// This should only be used internally
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
