#pragma once

#include <fgla/backends/vulkan/ext/windowing/window.hpp>
#include <fgla/ext/windowing/surface.hpp>
#include <vulkan/vulkan.h>

namespace fgla::backends::vulkan::ext::windowing {

struct SurfaceImpl : fgla::ext::windowing::Surface::Impl {
  SurfaceImpl(WindowImpl &window, const fgla::Instance &instance);

  inline VkSurfaceKHR get_surface() { return this->surface; }

  virtual std::optional<Error>
  configure(fgla::Device &device,
            const fgla::ext::windowing::Surface::Configuration &configuration) override;
  virtual fgla::ext::windowing::Surface::Capabilities
  get_capabilities(const Adapter &adapter) override;

  bool is_ok() const;

  virtual ~SurfaceImpl() override;

private:
  VkSurfaceKHR surface;
  VkSwapchainKHR swapchain;
  VkDevice device;
  VkInstance instance;
};
} // namespace fgla::backends::vulkan::ext::windowing
