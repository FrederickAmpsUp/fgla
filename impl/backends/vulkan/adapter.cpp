#include <fgla/backends/vulkan/adapter.hpp>
#include <fgla/backends/vulkan/device.hpp>
#include <fgla/backends/vulkan/queue.hpp>
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

tl::expected<Device, Error>
AdapterImpl::create_device(const Device::Descriptor &descriptor,
                           std::initializer_list<Queue::Request> queue_requests) {
  QueueAllocator queue_allocator = QueueAllocator(queue_requests, this->physical_device);

  std::unique_ptr<DeviceImpl> impl =
      std::make_unique<DeviceImpl>(*this, descriptor, queue_allocator);
  if (!impl->is_ok()) return tl::make_unexpected(Error(0));
  return std::move(Device::from_raw(std::move(impl)));
}

Adapter::Info AdapterImpl::get_info() const {
  Adapter::Info info;

  VkPhysicalDeviceProperties device_props;
  vkGetPhysicalDeviceProperties(this->physical_device, &device_props);

  info.device_name = std::string(device_props.deviceName);

  return info;
}

} // namespace fgla::backends::vulkan
