#include <fgla/backends/vulkan/ext/windowing/surface.hpp>
#include <fgla/backends/vulkan/instance.hpp>
#include <fgla/internal.hpp>
#include <spdlog/spdlog.h>

namespace fgla::backends::vulkan::ext::windowing {

SurfaceImpl::SurfaceImpl(WindowImpl &window, const fgla::Instance &instance) {
  static auto logger = spdlog::get("fgla::backends::vulkan");
  this->instance = dynamic_cast<InstanceImpl *>(fgla::internal::ImplAccessor::get_impl(instance))
                       ->get_instance();
  VkResult res =
      glfwCreateWindowSurface(this->instance, window.get_window(), nullptr, &this->surface);

  if (res != VK_SUCCESS) {
    logger->error("Failed to create Vulkan surface");
    this->surface = VK_NULL_HANDLE;
  } else {
    logger->info("Vulkan surface created.");
  }
}

bool SurfaceImpl::is_ok() const { return this->surface != VK_NULL_HANDLE; }

SurfaceImpl::~SurfaceImpl() {
  if (this->instance && this->surface) vkDestroySurfaceKHR(this->instance, this->surface, nullptr);
}
} // namespace fgla::backends::vulkan::ext::windowing
