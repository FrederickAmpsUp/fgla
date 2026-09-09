#include <fgla/backends/vulkan/device.hpp>
#include <fgla/backends/vulkan/memory.hpp>

namespace fgla::backends::vulkan {

Result<Memory::AccessMut> MemoryImpl::access() {
  return Memory::AccessMut::from_impl(std::make_unique<MemoryAccessImpl>(
      this->data, this->device.get_device(), this->device_memory, this->offset,
      this->size));
}

Result<Memory::AccessConst> MemoryImpl::access() const {
  return (Memory::AccessConst)Memory::AccessConst::from_impl(
      std::make_unique<MemoryAccessImpl>(this->data, this->device.get_device(),
                                         this->device_memory, this->offset,
                                         this->size));
}

MemoryImpl::~MemoryImpl() { this->device.free_memory(*this); }

MemoryAccessImpl::MemoryAccessImpl(void *data, VkDevice device,
                                   VkDeviceMemory memory, VkDeviceSize offset,
                                   VkDeviceSize size)
    : data(data), device(device) {
  this->memory_range = {};
  this->memory_range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
  this->memory_range.memory = memory;
  this->memory_range.offset = offset;
  this->memory_range.size = size;
}

const std::byte *MemoryAccessImpl::read() const {
  if (this->needs_invalidate)
    vkInvalidateMappedMemoryRanges(this->device, 1, &this->memory_range);

  this->needs_invalidate = false;
  return (const std::byte *)this->data;
}

std::byte *MemoryAccessImpl::write() {
  this->needs_flush = true;
  return (std::byte *)this->data;
}

MemoryAccessImpl::~MemoryAccessImpl() {
  if (this->needs_flush) {
    vkFlushMappedMemoryRanges(this->device, 1, &this->memory_range);
  }
}

} // namespace fgla::backends::vulkan
