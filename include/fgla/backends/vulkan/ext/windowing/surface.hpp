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
            const fgla::ext::windowing::Surface::Configuration &configuration)
      override;
  virtual fgla::ext::windowing::Surface::Capabilities
  get_capabilities(const Adapter &adapter) override;

  virtual fgla::Result<std::reference_wrapper<fgla::Image>>
  get_current_image(const fgla::Queue &queue) override;

  virtual std::optional<Error>
  present(fgla::Queue &present_queue, fgla::Image &&image,
          std::initializer_list<fgla::Completion> wait_completions) override;

  virtual void cleanup() override;

  bool is_ok() const;

  virtual ~SurfaceImpl() override;

private:
  VkSemaphore get_semaphore();

  VkSurfaceKHR surface;
  VkSwapchainKHR swapchain;
  VkDevice device;
  VkInstance instance;
  uint32_t semaphore_index = 0;
  std::vector<VkSemaphore> semaphore_pool;
  std::vector<fgla::Image> swapchain_images;
};
} // namespace fgla::backends::vulkan::ext::windowing
