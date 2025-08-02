#pragma once

#include <fgla/backends/vulkan/ext/windowing/window.hpp>
#include <fgla/ext/windowing/surface.hpp>
#include <vulkan/vulkan.h>

namespace fgla::backends::vulkan::ext::windowing {

struct SurfaceImpl : fgla::ext::windowing::Surface::Impl {
  SurfaceImpl(WindowImpl& window, const fgla::Instance& instance);

  bool is_ok() const;

  virtual ~SurfaceImpl() override;
private:
  VkSurfaceKHR surface;
  VkInstance instance;
};
}
