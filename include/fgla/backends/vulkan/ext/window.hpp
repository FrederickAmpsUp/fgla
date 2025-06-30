#pragma once

#include <GLFW/glfw3.h>
#include <fgla/ext/windowing/extension.hpp>

namespace fgla::backends::vulkan::ext::windowing {

class WindowExtensionImpl : public fgla::ext::windowing::WindowExtension {
public:
  virtual tl::expected<fgla::ext::windowing::Window, Error>
  create_window(const fgla::ext::windowing::Window::Descriptor &descriptor) override;
  virtual ~WindowExtensionImpl() override = default;
};

extern WindowExtensionImpl window_extension_impl;

struct WindowImpl : public fgla::ext::windowing::Window::Impl {
  WindowImpl(const fgla::ext::windowing::Window::Descriptor &);

  virtual void poll_events() override;
  virtual bool is_open() override;

  bool is_ok() const;

  virtual ~WindowImpl() override;

private:
  GLFWwindow *window;
};

bool init_windowing();
bool add_window();
void remove_window();

} // namespace fgla::backends::vulkan::ext::windowing
