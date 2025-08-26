#include <fgla/backends/vulkan/ext/windowing/surface.hpp>
#include <fgla/backends/vulkan/instance.hpp>
#include <fgla/backends/vulkan/adapter.hpp>
#include <fgla/backends/vulkan/util.hpp>
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

void SurfaceImpl::configure(fgla::Device &device,
                            const fgla::ext::windowing::Surface::Configuration &configuration) {
  static auto logger = spdlog::get("fgla::backends::vulkan");
  logger->warn("Unimplemented SurfaceImpl::configure()!");
}

fgla::ext::windowing::Surface::Capabilities SurfaceImpl::get_capabilities(const Adapter &adapter) {
  VkPhysicalDevice phys_dev = static_cast<AdapterImpl *>(fgla::internal::ImplAccessor::get_impl(adapter))->get_physical_device();

  fgla::ext::windowing::Surface::Capabilities caps = {};

  uint32_t n_formats = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(phys_dev, this->surface, &n_formats, nullptr);
  
  if (n_formats > 0) {
    std::vector<VkSurfaceFormatKHR> formats(n_formats);
    vkGetPhysicalDeviceSurfaceFormatsKHR(phys_dev, this->surface, &n_formats, formats.data());

    for (const auto &format : formats) {
      fgla::TextureFormat fmt = devulkanize(format.format);
    }
  }

  return fgla::ext::windowing::Surface::Capabilities{};
}

bool SurfaceImpl::is_ok() const { return this->surface != VK_NULL_HANDLE; }

SurfaceImpl::~SurfaceImpl() {
  if (this->instance && this->surface) vkDestroySurfaceKHR(this->instance, this->surface, nullptr);
}
} // namespace fgla::backends::vulkan::ext::windowing
