#include <fgla/backends/vulkan/backend.hpp>
#include <fgla/backends/vulkan/instance.hpp>
#include <fgla/util.hpp>
#include <iostream>
#include <spdlog/spdlog.h>

namespace fgla::backends::vulkan {

extern "C" bool fgla_backends_vulkan_is_available() {
  VkInstanceCreateInfo instance_info = {};
  instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

  instance_info.enabledExtensionCount = 0; // TODO: GLFW extensions if we need the windowing feature
  instance_info.enabledLayerCount = 0;

  // allegedly i'll have to update this for MacOS support
  VkInstance instance;
  VkResult result = vkCreateInstance(&instance_info, nullptr, &instance);
  if (result != VK_SUCCESS) {
    spdlog::warn("fgla::backends::vulkan::is_available(): failed to create Vulkan instance!");
    return false; // if we can't create the instance, we definitely can't use Vulkan
  }

  uint32_t physical_device_count = 0;
  vkEnumeratePhysicalDevices(instance, &physical_device_count, nullptr);

  if (physical_device_count == 0) {
    vkDestroyInstance(instance, nullptr);
    spdlog::warn("fgla::backends::vulkan::is_available(): no adapters available!");
    return false;
  }

  // TODO: device suitability checks

  vkDestroyInstance(instance, nullptr);
  return true;
}

extern "C" Instance *fgla_backends_vulkan_create_instance(const Instance::Descriptor *descriptor) {
  std::unique_ptr<InstanceImpl> impl = std::make_unique<InstanceImpl>(*descriptor);
  if (!impl->is_ok()) return nullptr;
  auto instance = Instance::from_raw(std::move(impl));
  return new Instance(std::move(instance));
}

static const fgla::backend::BackendRegistrar registrar = {
    fgla::backend::Backend{.uuid = UUID,
                           .name = "Vulkan",
                           .is_available = &fgla_backends_vulkan_is_available,
                           .create_instance = &fgla_backends_vulkan_create_instance}};

} // namespace fgla::backends::vulkan
