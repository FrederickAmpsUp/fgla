#pragma once

#include <fgla/ext/windowing/extension.hpp>

namespace fgla::backends::vulkan::ext::windowing {

class WindowExtensionImpl : public fgla::ext::windowing::WindowExtension {
public:
  virtual fgla::ext::windowing::Window
  create_window(const fgla::ext::windowing::Window::Descriptor &descriptor) override;
  virtual ~WindowExtensionImpl() override = default;
};

extern WindowExtensionImpl window_extension_impl;

struct WindowImpl : public fgla::ext::windowing::Window::Impl {
  WindowImpl(const fgla::ext::windowing::Window::Descriptor &);
  virtual ~WindowImpl() override = default;
};

} // namespace fgla::backends::vulkan::ext::windowing
