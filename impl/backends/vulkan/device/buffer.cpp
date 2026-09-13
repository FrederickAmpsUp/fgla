#include <fgla/backends/vulkan/buffer.hpp>
#include <fgla/backends/vulkan/device.hpp>
#include <fgla/backends/vulkan/memory.hpp>
#include <fgla/backends/vulkan/util.hpp>

namespace fgla::backends::vulkan {

Result<Buffer> DeviceImpl::create_buffer(const Buffer::Descriptor &desc) {
  VkBufferCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

  create_info.size = desc.memory.size;
  create_info.usage = vulkanize(desc.usage);
  create_info.sharingMode = this->used_queue_family_indices.size() > 1
                                ? VK_SHARING_MODE_CONCURRENT
                                : VK_SHARING_MODE_EXCLUSIVE;
  create_info.queueFamilyIndexCount = this->used_queue_family_indices.size();
  create_info.pQueueFamilyIndices = this->used_queue_family_indices.data();

  VmaAllocationCreateInfo allocation_create_info =
      make_allocation_create_info(desc.memory);

  VkBuffer buffer;
  VmaAllocation allocation;
  VmaAllocationInfo allocation_info;

  VkResult res =
      vmaCreateBuffer(this->allocator, &create_info, &allocation_create_info,
                      &buffer, &allocation, &allocation_info);
  if (res != VK_SUCCESS) {
    return Error(1, "Failed to create Vulkan buffer");
  }

  Memory memory = Memory::from_impl(std::make_unique<MemoryImpl>(
      allocation, allocation_info.pMappedData, *this));

  return Buffer::from_impl(
      std::make_unique<BufferImpl>(buffer, std::move(memory), this->device));
}

} // namespace fgla::backends::vulkan
