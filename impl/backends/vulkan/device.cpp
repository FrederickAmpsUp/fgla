#include <fgla/backends/vulkan/device.hpp>
#include <spdlog/spdlog.h>

namespace fgla::backends::vulkan {

// TODO: move this into AdapterImpl
DeviceImpl::DeviceImpl(AdapterImpl &adapter, const Device::Descriptor &descriptor,
                       QueueAllocator &queue_allocator) {
  static auto logger = spdlog::get("fgla::backends::vulkan");

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

  VkResult result =
      vkCreateDevice(adapter.get_physical_device(), &create_info, nullptr, &this->device);

  if (result != VK_SUCCESS) {
    logger->error("Vulkan device creation failed, error code {:X}", (unsigned int)result);
    this->device = VK_NULL_HANDLE;
    return;
  }

  this->physical_device = adapter.get_physical_device();

  logger->info("Vulkan device created.");

  this->queues = queue_allocator.get_queues(*this);
}
bool DeviceImpl::is_ok() const { return this->device != VK_NULL_HANDLE; }

Queue *DeviceImpl::get_queue(Queue::Type type, uint32_t index) {
  std::pair<Queue::Type, uint32_t> key = {type, index};

  if (this->queues.find(key) == this->queues.end()) {
    return nullptr;
  }

  return &this->queues.at({type, index});
}

DeviceImpl::~DeviceImpl() { vkDestroyDevice(this->device, nullptr); }
} // namespace fgla::backends::vulkan
