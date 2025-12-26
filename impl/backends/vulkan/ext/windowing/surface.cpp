#include <fgla/backends/vulkan/adapter.hpp>
#include <fgla/backends/vulkan/device.hpp>
#include <fgla/backends/vulkan/ext/windowing/image.hpp>
#include <fgla/backends/vulkan/ext/windowing/surface.hpp>
#include <fgla/backends/vulkan/instance.hpp>
#include <fgla/backends/vulkan/util.hpp>
#include <fgla/internal.hpp>
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <unordered_map>
#include <unordered_set>

using namespace fgla::ext::windowing;

namespace fgla::backends::vulkan::ext::windowing {

// returns the best available VkPresentModeKHR for the Surface::PresentMode, or
// VK_PRESENT_MODE_MAX_ENUM_KHR if none are supported
static VkPresentModeKHR pick_present_mode(VkPhysicalDevice phys_dev, VkSurfaceKHR surface,
                                          Surface::PresentMode present_mode) {
  VkPresentModeKHR pm = VK_PRESENT_MODE_MAX_ENUM_KHR;

  uint32_t n_present_modes = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(phys_dev, surface, &n_present_modes, nullptr);

  std::vector<VkPresentModeKHR> present_modes(n_present_modes);
  vkGetPhysicalDeviceSurfacePresentModesKHR(phys_dev, surface, &n_present_modes,
                                            present_modes.data());

  std::unordered_set<VkPresentModeKHR> unique_present_modes(present_modes.begin(),
                                                            present_modes.end());

  // fine, mom, i'll clean my code...
  std::unordered_map<Surface::PresentMode, std::vector<VkPresentModeKHR>> present_mode_selections =
      {
          {Surface::PresentMode::FIFO, {VK_PRESENT_MODE_FIFO_KHR}},
          {Surface::PresentMode::MAILBOX, {VK_PRESENT_MODE_MAILBOX_KHR}},
          {Surface::PresentMode::IMMEDIATE, {VK_PRESENT_MODE_IMMEDIATE_KHR}},
          {Surface::PresentMode::AUTO_VSYNC,
           {VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_FIFO_KHR}},
          {Surface::PresentMode::AUTO_NO_VSYNC,
           {VK_PRESENT_MODE_FIFO_RELAXED_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR,
            VK_PRESENT_MODE_FIFO_KHR}},
      };

  if (present_mode_selections.find(present_mode) != present_mode_selections.end())
    for (auto mode : present_mode_selections[present_mode]) {
      if (unique_present_modes.find(mode) != unique_present_modes.end()) {
        pm = mode;
        break;
      }
    }

  return pm;
}

VkExtent2D pick_swap_extent(VkSurfaceCapabilitiesKHR caps, Extent2d fb_size) {
  VkExtent2D extent;
  if (caps.currentExtent.width != UINT32_MAX) {
    extent = caps.currentExtent;
  } else {
    extent.width = std::clamp(fb_size.width, caps.minImageExtent.width, caps.maxImageExtent.width);
    extent.height =
        std::clamp(fb_size.height, caps.minImageExtent.height, caps.maxImageExtent.height);
  }

  return extent;
}

std::vector<uint32_t> find_graphics_present_families(VkPhysicalDevice phys_dev,
                                                     VkSurfaceKHR surface) {
  uint32_t n_queue_families = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(phys_dev, &n_queue_families, nullptr);
  std::vector<VkQueueFamilyProperties> queue_family_props(n_queue_families);
  vkGetPhysicalDeviceQueueFamilyProperties(phys_dev, &n_queue_families, queue_family_props.data());

  std::vector<uint32_t> queue_families;
  for (uint32_t i = 0; i < n_queue_families; i++) {
    VkBool32 supported;
    vkGetPhysicalDeviceSurfaceSupportKHR(phys_dev, i, surface, &supported);
    if (queue_family_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT || supported) {
      queue_families.push_back(i);
    }
  }

  return std::move(queue_families);
}

SurfaceImpl::SurfaceImpl(WindowImpl &window, const fgla::Instance &instance) {
  static auto logger = spdlog::get("fgla::backends::vulkan");
  this->instance = dynamic_cast<InstanceImpl *>(fgla::internal::ImplAccessor::get_impl(instance))
                       ->get_instance();
  VkResult res =
      glfwCreateWindowSurface(this->instance, window.get_window(), nullptr, &this->surface);

  this->swapchain = VK_NULL_HANDLE;
  this->device = VK_NULL_HANDLE;
  if (res != VK_SUCCESS) {
    logger->error("Failed to create Vulkan surface.");
    this->surface = VK_NULL_HANDLE;
  } else {
    logger->info("Vulkan surface created.");
  }
}

// TODO: custom error enum here
std::optional<Error>
SurfaceImpl::configure(fgla::Device &device,
                       const fgla::ext::windowing::Surface::Configuration &configuration) {
  static auto logger = spdlog::get("fgla::backends::vulkan");

  logger->info("Configuring surface:");
  logger->info(" - Format: \"{}\"", configuration.format.to_string());
  logger->info(" - Present Mode: \"{}\"", magic_enum::enum_name(configuration.present_mode));
  logger->info(" - Size: {}x{}", configuration.size.width, configuration.size.height);

  VkPhysicalDevice phys_dev =
      static_cast<DeviceImpl *>(fgla::internal::ImplAccessor::get_impl(device))
          ->get_physical_device();
  VkDevice logi_dev =
      static_cast<DeviceImpl *>(fgla::internal::ImplAccessor::get_impl(device))->get_device();

  VkFormat fmt = vulkanize(configuration.format); // ez pz
  VkPresentModeKHR pm = pick_present_mode(phys_dev, this->surface, configuration.present_mode);

  if (pm == VK_PRESENT_MODE_MAX_ENUM_KHR) {
    auto name = magic_enum::enum_name(configuration.present_mode);
    logger->error("Requested present mode {} unsupported.", name);
    return Error(0, fmt::format("Requested present mode {} unsupported", name));
  }

  VkSurfaceCapabilitiesKHR caps;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(phys_dev, this->surface, &caps);

  VkExtent2D extent = pick_swap_extent(caps, configuration.size);

  uint32_t n_images = caps.minImageCount;
  if (caps.maxImageCount != caps.minImageCount) {
    n_images++;
  }

  VkSwapchainCreateInfoKHR create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  create_info.surface = this->surface;

  create_info.minImageCount = n_images;
  create_info.imageFormat = fmt;
  create_info.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
  create_info.imageExtent = extent;

  create_info.imageArrayLayers = 1;
  create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  // might improve this in the future, for now just assume any graphics or present-capable family
  // can access the swapchain images
  std::vector<uint32_t> queue_families = find_graphics_present_families(phys_dev, this->surface);

  // if the images might be accessed in multiple families, we have to use concurrent mode, otherwise
  // exclusive mode is used for 1% better frame times :)
  create_info.imageSharingMode =
      (queue_families.size() > 1) ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
  create_info.queueFamilyIndexCount = static_cast<uint32_t>(queue_families.size());
  create_info.pQueueFamilyIndices = queue_families.data();

  create_info.preTransform = caps.currentTransform;
  create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

  create_info.presentMode = pm;
  create_info.clipped = VK_TRUE;

  create_info.oldSwapchain = this->swapchain;
  VkSwapchainKHR old_swapchain = this->swapchain;

  VkDevice dev =
      static_cast<DeviceImpl *>(fgla::internal::ImplAccessor::get_impl(device))->get_device();

  VkResult res = vkCreateSwapchainKHR(dev, &create_info, nullptr, &this->swapchain);

  if (old_swapchain != VK_NULL_HANDLE) {
    vkDestroySwapchainKHR(logi_dev, old_swapchain, nullptr);
  }

  if (res != VK_SUCCESS) {
    logger->error("Failed to create Vulkan swapchain (error code {}).", (uint64_t)res);
    this->swapchain = VK_NULL_HANDLE;
    return Error(1, "Failed to create Vulkan swapchain");
  }
  logger->info("Vulkan swapchain created.");
  this->device = dev;

  n_images = 0;
  vkGetSwapchainImagesKHR(logi_dev, this->swapchain, &n_images, nullptr);
  std::vector<VkImage> images(n_images);
  this->swapchain_images.resize(n_images);
  vkGetSwapchainImagesKHR(logi_dev, this->swapchain, &n_images, images.data());

  for (int i = 0; i < n_images; ++i) {
    std::unique_ptr<fgla::Image::Impl> image =
        std::make_unique<SwapchainImageImpl>(images[i], logi_dev);
    this->swapchain_images[i] = fgla::Image::from_raw(std::move(image));
  }

  logger->info("Retrieved {} swapchain images.", n_images);

  return {};
}

fgla::ext::windowing::Surface::Capabilities SurfaceImpl::get_capabilities(const Adapter &adapter) {
  VkPhysicalDevice phys_dev =
      static_cast<AdapterImpl *>(fgla::internal::ImplAccessor::get_impl(adapter))
          ->get_physical_device();

  fgla::ext::windowing::Surface::Capabilities caps = {};

  uint32_t n_formats = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(phys_dev, this->surface, &n_formats, nullptr);

  if (n_formats > 0) {
    std::unordered_set<TextureFormat> unique_formats;
    std::vector<VkSurfaceFormatKHR> formats(n_formats);
    vkGetPhysicalDeviceSurfaceFormatsKHR(phys_dev, this->surface, &n_formats, formats.data());

    for (const auto &format : formats) {
      TextureFormat fmt = devulkanize(format.format);
      // only support sRGB for now, due to color space stuff. want to add HDR later.
      if (fmt != TextureFormat::UNDEFINED && fmt.is_srgb()) unique_formats.insert(fmt);
    }
    caps.formats = std::vector(unique_formats.begin(), unique_formats.end());
  }

  uint32_t n_present_modes = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(phys_dev, this->surface, &n_present_modes, nullptr);

  if (n_present_modes > 0) {
    std::unordered_set<Surface::PresentMode> unique_present_modes;
    std::vector<VkPresentModeKHR> present_modes(n_present_modes);
    vkGetPhysicalDeviceSurfacePresentModesKHR(phys_dev, this->surface, &n_present_modes,
                                              present_modes.data());

    for (const auto &present_mode : present_modes) {
      Surface::PresentMode pm =
          Surface::PresentMode::AUTO_VSYNC; // just used as a "not found" flag here
      switch (present_mode) {
      case VK_PRESENT_MODE_FIFO_KHR:
        pm = Surface::PresentMode::FIFO;
        break;
      case VK_PRESENT_MODE_MAILBOX_KHR:
        pm = Surface::PresentMode::MAILBOX;
        break;
      case VK_PRESENT_MODE_IMMEDIATE_KHR:
        pm = Surface::PresentMode::IMMEDIATE;
        break;
      default:
        break;
      }
      if (pm != Surface::PresentMode::AUTO_VSYNC) unique_present_modes.insert(pm);
    }
    caps.present_modes = std::vector(unique_present_modes.begin(), unique_present_modes.end());
  }

  return caps;
}

fgla::Result<std::reference_wrapper<fgla::Image>>
SurfaceImpl::get_current_image(const fgla::Queue &device) {
  return Error(0, "Unimplemented");
}

bool SurfaceImpl::is_ok() const { return this->surface != VK_NULL_HANDLE; }

SurfaceImpl::~SurfaceImpl() {
  if (this->device && this->swapchain)
    vkDestroySwapchainKHR(this->device, this->swapchain, nullptr);
  if (this->instance && this->surface) vkDestroySurfaceKHR(this->instance, this->surface, nullptr);
}
} // namespace fgla::backends::vulkan::ext::windowing
