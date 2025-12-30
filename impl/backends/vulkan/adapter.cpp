#include <fgla/backends/vulkan/adapter.hpp>
#include <fgla/backends/vulkan/device.hpp>
#include <fgla/backends/vulkan/queue.hpp>
#include <fgla/error.hpp>
#include <spdlog/spdlog.h>

namespace fgla::backends::vulkan {

QueueFamilyIndices QueueFamilyIndices::find(VkPhysicalDevice device) {
  uint32_t n_queue_families = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &n_queue_families, nullptr);

  std::vector<VkQueueFamilyProperties> queue_families(n_queue_families);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &n_queue_families, queue_families.data());

  QueueFamilyIndices indices;

  uint32_t i = 0;
  for (const auto &queue_family : queue_families) {
    if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphics_family = i;
    }

    ++i;
  }

  return indices;
}

Result<Device> AdapterImpl::create_device(const Device::Descriptor &descriptor,
                                          std::initializer_list<Queue::Request> queue_requests) {
  static auto logger = spdlog::get("fgla::backends::vulkan");

  QueueAllocator queue_allocator = QueueAllocator(queue_requests, this->physical_device);

  const std::vector<VkDeviceQueueCreateInfo> &queue_create_infos =
      queue_allocator.get_queue_create_infos();

  VkPhysicalDeviceFeatures device_features = {}; // TODO

  VkDeviceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

  create_info.pEnabledFeatures = &device_features;

  create_info.queueCreateInfoCount = queue_create_infos.size();
  create_info.pQueueCreateInfos = queue_create_infos.data();

  std::vector<const char *> extensions;

#ifdef FGLA_VK_EXT_WINDOWING
  extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
#endif

  create_info.enabledExtensionCount = extensions.size();
  create_info.ppEnabledExtensionNames = extensions.data();

  VkDevice device;

  VkResult result = vkCreateDevice(this->physical_device, &create_info, nullptr, &device);

  if (result != VK_SUCCESS) {
    return Error(1, "Failed to create Vulkan device");
  }

  logger->info("Vulkan device created.");

  QueueAllocator::Queues queues = queue_allocator.get_queues(device);

  return Device::from_raw(
      std::make_unique<DeviceImpl>(device, this->physical_device, std::move(queues)));
}

Adapter::Info AdapterImpl::get_info() const {
  Adapter::Info info;

  VkPhysicalDeviceProperties device_props;
  vkGetPhysicalDeviceProperties(this->physical_device, &device_props);

  info.device_name = std::string(device_props.deviceName);

  return info;
}

} // namespace fgla::backends::vulkan
