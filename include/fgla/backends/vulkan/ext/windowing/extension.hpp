#pragma once

#include <fgla/ext/windowing/extension.hpp>

namespace fgla::backends::vulkan::ext::windowing {

class WindowExtensionImpl : public fgla::ext::windowing::WindowExtension {
public:
  virtual tl::expected<fgla::ext::windowing::Window, Error>
  create_window(const fgla::ext::windowing::Window::Descriptor &descriptor) override;
  virtual std::function<bool(const Adapter&)> surface_support_filter(const fgla::ext::windowing::Surface &) override;
  virtual ~WindowExtensionImpl() override = default;
};

extern WindowExtensionImpl window_extension_impl;
} // namespace fgla::backends::vulkan::ext::windowing
