#if FGLA_VK_EXT_WINDOWING
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <fgla/backends/vulkan/ext/windowing/extension.hpp>
#endif

#include <fgla/backends/vulkan/adapter.hpp>
#include <fgla/backends/vulkan/backend.hpp>
#include <fgla/backends/vulkan/instance.hpp>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace fgla::backends::vulkan {

InstanceImpl::InstanceImpl(const Instance::Descriptor &descriptor) {
  static auto logger = spdlog::stdout_color_mt("fgla::backends::vulkan");
  logger->set_level(spdlog::level::debug); // TODO: modify this

  VkApplicationInfo app_info = {};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

  app_info.applicationVersion = VK_MAKE_VERSION(
      descriptor.app_version.major, descriptor.app_version.minor, descriptor.app_version.patch);
  app_info.pApplicationName = descriptor.app_name.c_str();

  logger->info("Creating FGLA Vulkan instance for app \"{}\" (v{}.{}.{}).", descriptor.app_name,
               descriptor.app_version.major, descriptor.app_version.minor,
               descriptor.app_version.patch);

  app_info.pEngineName = "FGLA";
  app_info.engineVersion = VK_MAKE_VERSION(0, 0, 1);

  app_info.apiVersion = VK_API_VERSION_1_3;

  VkInstanceCreateInfo instance_info = {};
  instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

  instance_info.pApplicationInfo = &app_info;

  std::vector<const char *> extensions;

#if FGLA_VK_EXT_WINDOWING
  uint32_t n_glfw_extensions = 0;
  const char **glfw_extensions;

  glfwInit();
  glfw_extensions = glfwGetRequiredInstanceExtensions(&n_glfw_extensions);

  extensions.insert(extensions.end(), glfw_extensions, glfw_extensions + n_glfw_extensions);
#endif

  instance_info.enabledExtensionCount = extensions.size();
  instance_info.ppEnabledExtensionNames = extensions.data();

  std::vector<const char *> layers;
#ifndef NDEBUG
  layers.push_back("VK_LAYER_KHRONOS_validation"); // should query if it's actually available
#endif

  instance_info.enabledLayerCount = layers.size();
  instance_info.ppEnabledLayerNames = layers.data();

  VkResult result = vkCreateInstance(&instance_info, nullptr, &this->instance);
  if (result != VK_SUCCESS) {
    logger->error("Vulkan instance creation failed, error code {:X}", (unsigned int)result);
    this->instance = VK_NULL_HANDLE;
    return;
  }

  logger->info("Vulkan instance created.");
}
bool InstanceImpl::is_ok() const { return this->instance != VK_NULL_HANDLE; }

static int score_physical_device(VkPhysicalDevice device, const Adapter::Descriptor &descriptor) {
  VkPhysicalDeviceProperties props;
  vkGetPhysicalDeviceProperties(device, &props);

  VkPhysicalDeviceFeatures features;
  vkGetPhysicalDeviceFeatures(device, &features);

  QueueFamilyIndices indices = QueueFamilyIndices::find(device);

  if (!indices.graphics_family.has_value()) {
    return 0; // we need a device that can draw stuff, probably
  }

  int score = 0;

  // Prefer discrete GPUs
  if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
    score += 10000;
  } else if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
    score += 1000;
  }

  // Add score based on VRAM (in MB)
  VkPhysicalDeviceMemoryProperties memProps;
  vkGetPhysicalDeviceMemoryProperties(device, &memProps);

  VkDeviceSize totalVRAM = 0;
  for (uint32_t i = 0; i < memProps.memoryHeapCount; i++) {
    if (memProps.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
      totalVRAM += memProps.memoryHeaps[i].size;
    }
  }
  score += static_cast<int>(totalVRAM / (4 * 1024 * 1024));

  // need more scoring later
  return score;
}

std::vector<Adapter> InstanceImpl::enumerate_adapters() {
  std::vector<Adapter> adapters;

  static auto logger = spdlog::get("fgla::backends::vulkan");

  VkInstance instance = this->instance;

  uint32_t n_devices = 0;
  vkEnumeratePhysicalDevices(instance, &n_devices, nullptr);

  if (n_devices == 0) {
    return adapters;
  }

  std::vector<VkPhysicalDevice> devices(n_devices);
  vkEnumeratePhysicalDevices(instance, &n_devices, devices.data());

  logger->info("{} physical devices available:", n_devices);

  for (VkPhysicalDevice candidate : devices) {
    VkPhysicalDeviceProperties candidate_props;
    vkGetPhysicalDeviceProperties(candidate, &candidate_props);

    std::string vendor_name = fmt::format("Unknown, 0x{:4X}", candidate_props.vendorID);

    switch (candidate_props.vendorID) {
    case 0x10DE:
      vendor_name = "NVIDIA";
      break;
    case 0x1002:
      vendor_name = "AMD";
      break;
    case 0x8086:
      vendor_name = "Intel";
      break;
    }

    logger->info(" - \"{}\", vendor \"{}\"", candidate_props.deviceName, vendor_name);

    auto adapter_impl = std::make_unique<AdapterImpl>(candidate);
    adapters.push_back(Adapter::from_raw(std::move(adapter_impl)));
  }

  return adapters;
}

std::function<int(const Adapter &)>
InstanceImpl::get_adapter_scorer(const Adapter::Descriptor &descriptor) {
  return [&](const Adapter &a) -> int {
    auto impl = dynamic_cast<AdapterImpl *>(fgla::internal::ImplAccessor::get_impl(a));
    return score_physical_device(impl->get_physical_device(), descriptor);
  };
}

const backend::Backend &InstanceImpl::get_backend() {
  return fgla::backend::get_registry().at(UUID);
}

void *InstanceImpl::get_extension(extension::ExtensionUUID uuid) {
#ifdef FGLA_VK_EXT_WINDOWING
  if (uuid == fgla::ext::windowing::WindowingExtension::UUID) {
    fgla::ext::windowing::WindowingExtension &ext = ext::windowing::windowing_extension_impl;
    return static_cast<void *>(&ext);
  }
#endif

  return nullptr;
}

InstanceImpl::~InstanceImpl() { vkDestroyInstance(this->instance, nullptr); }

} // namespace fgla::backends::vulkan
