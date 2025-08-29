#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <fgla/ext/windowing/extension.hpp>

namespace fgla::backends::vulkan::ext::windowing {

struct WindowImpl : public fgla::ext::windowing::Window::Impl {
  WindowImpl(const fgla::ext::windowing::Window::Descriptor &);

  virtual void poll_events() override;
  virtual bool is_open() override;

  virtual Result<fgla::ext::windowing::Surface>
  create_surface(const fgla::Instance &instance) override;

  virtual Extent2d get_framebuffer_size() const override;

  bool is_ok() const;

  GLFWwindow *get_window() { return this->window; }

  virtual ~WindowImpl() override;

private:
  GLFWwindow *window;
};

bool init_windowing();
bool add_window();
void remove_window();

} // namespace fgla::backends::vulkan::ext::windowing
