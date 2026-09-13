#include <fgla/backends/vulkan/device.hpp>
#include <fgla/backends/vulkan/memory.hpp>

namespace fgla::backends::vulkan {

Result<Memory::AccessMut> MemoryImpl::access() {
  return Memory::AccessMut::from_impl(std::make_unique<MemoryAccessImpl>(
      this->data, this->device.get_allocator(), allocation));
}

Result<Memory::AccessConst> MemoryImpl::access() const {
  return (Memory::AccessConst)Memory::AccessConst::from_impl(
      std::make_unique<MemoryAccessImpl>(
          this->data, this->device.get_allocator(), allocation));
}

VmaAllocator MemoryImpl::get_allocator() const {
  return this->device.get_allocator();
}

MemoryImpl::~MemoryImpl() {}

MemoryAccessImpl::MemoryAccessImpl(void *data, VmaAllocator allocator,
                                   VmaAllocation allocation)
    : data(data), allocator(allocator), allocation(allocation) {}

const std::byte *MemoryAccessImpl::read() const {
  if (this->needs_invalidate)
    vmaInvalidateAllocation(this->allocator, this->allocation, 0,
                            VK_WHOLE_SIZE);

  this->needs_invalidate = false;
  return (const std::byte *)this->data;
}

std::byte *MemoryAccessImpl::write() {
  this->needs_flush = true;
  return (std::byte *)this->data;
}

MemoryAccessImpl::~MemoryAccessImpl() {
  if (this->needs_flush) {
    vmaFlushAllocation(this->allocator, this->allocation, 0, VK_WHOLE_SIZE);
  }
}

VmaAllocationCreateInfo
make_allocation_create_info(const Memory::Descriptor &desc) {
  VmaAllocationCreateInfo allocation_create_info = {};
  allocation_create_info.usage = VMA_MEMORY_USAGE_AUTO;

  if (desc.cpu_access == Memory::CpuAccess::NONE) {
    allocation_create_info.preferredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
  } else {
    allocation_create_info.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT |
                                   VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
  }

  if ((desc.cpu_access & Memory::CpuAccess::READ) != Memory::CpuAccess::NONE) {
    allocation_create_info.preferredFlags |= VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
  }
  if ((desc.cpu_access & Memory::CpuAccess::WRITE) != Memory::CpuAccess::NONE) {
    allocation_create_info.preferredFlags |=
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
  }

  return allocation_create_info;
}

} // namespace fgla::backends::vulkan
