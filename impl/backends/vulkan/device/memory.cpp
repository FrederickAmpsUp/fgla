#include "fgla/backends/vulkan/memory.hpp"

#include <fgla/backends/vulkan/device.hpp>
#include <vulkan/vulkan_core.h>

namespace fgla::backends::vulkan {

// TODO: implement an arena allocator
// TODO: make HOST_CACHED / HOST_COHERENT non-required
static uint32_t find_memory_type(VkPhysicalDevice phys_dev,
                                 uint32_t type_filter,
                                 VkMemoryPropertyFlags props) {
  VkPhysicalDeviceMemoryProperties mem_properties;
  vkGetPhysicalDeviceMemoryProperties(phys_dev, &mem_properties);

  for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++) {
    if ((type_filter & (1 << i)) &&
        (mem_properties.memoryTypes[i].propertyFlags & props) == props) {
      return i;
    }
  }

  return UINT32_MAX;
}

Result<Memory> DeviceImpl::allocate_memory(VkMemoryRequirements requirements,
                                           Memory::CpuAccess cpu_access) {
  VkMemoryPropertyFlags props = 0;

  if (cpu_access != Memory::CpuAccess::NONE)
    props |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
  if ((cpu_access & Memory::CpuAccess::READ) != Memory::CpuAccess::NONE)
    props |= VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
  if ((cpu_access & Memory::CpuAccess::WRITE) != Memory::CpuAccess::NONE)
    props |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

  uint32_t memory_type = find_memory_type(this->physical_device,
                                          requirements.memoryTypeBits, props);
  if (memory_type == UINT32_MAX)
    return Error(1, "Failed to find compatible memory type");
  VkMemoryAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.allocationSize = requirements.size;
  alloc_info.memoryTypeIndex = memory_type;

  VkDeviceMemory device_memory;
  if (vkAllocateMemory(this->device, &alloc_info, nullptr, &device_memory) !=
      VK_SUCCESS) {
    return Error(2, "Failed to allocate Vulkan device memory");
  }

  void *data = nullptr;

  if (cpu_access != Memory::CpuAccess::NONE) {
    if (vkMapMemory(this->device, device_memory, 0, requirements.size, 0,
                    &data) != VK_SUCCESS) {
      vkFreeMemory(this->device, device_memory, nullptr);
      return Error(3, "Failed to map Vulkan device memory");
    }
  }

  return Memory::from_impl(std::make_unique<MemoryImpl>(
      device_memory, 0, requirements.size, data, *this));
}

void DeviceImpl::free_memory(const MemoryImpl &memory) {
  // fix, can't unmap if not mapped
  vkUnmapMemory(this->device, memory.get_device_memory());
  vkFreeMemory(this->device, memory.get_device_memory(), nullptr);
}

} // namespace fgla::backends::vulkan
