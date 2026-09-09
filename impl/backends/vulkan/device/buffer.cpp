#include "fgla/backends/vulkan/memory.hpp"

#include <fgla/backends/vulkan/buffer.hpp>
#include <fgla/backends/vulkan/device.hpp>
#include <fgla/backends/vulkan/util.hpp>

namespace fgla::backends::vulkan {

Result<Buffer> DeviceImpl::create_buffer(const Buffer::Descriptor &desc) {
  VkBufferCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

  create_info.size = desc.memory.size;
  create_info.usage = vulkanize(desc.usage);
  create_info.sharingMode = VK_SHARING_MODE_CONCURRENT;

  VkBuffer buffer;

  VkResult res = vkCreateBuffer(this->device, &create_info, nullptr, &buffer);
  if (res != VK_SUCCESS) {
    return Error(1, "Failed to create Vulkan buffer");
  }

  VkMemoryRequirements memory_requirements;
  vkGetBufferMemoryRequirements(this->device, buffer, &memory_requirements);

  Result<Memory> mem_res =
      this->allocate_memory(memory_requirements, desc.memory.cpu_access);

  if (mem_res.has_error()) {
    vkDestroyBuffer(this->device, buffer, nullptr);
    return Error(mem_res.error());
  }

  auto &impl = (*mem_res).to_impl<MemoryImpl>();

  res = vkBindBufferMemory(this->device, buffer, impl.get_device_memory(),
                           impl.get_offset());
  if (res != VK_SUCCESS) {
    vkDestroyBuffer(this->device, buffer, nullptr);
    this->free_memory(std::move(*mem_res));
    return Error(2, "Failed to bind Vulkan buffer memory");
  }

  return Buffer::from_impl(
      std::make_unique<BufferImpl>(buffer, std::move(*mem_res)));
}

} // namespace fgla::backends::vulkan
