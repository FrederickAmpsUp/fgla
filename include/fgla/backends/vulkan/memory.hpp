#pragma once

#include <fgla/memory.hpp>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

namespace fgla::backends::vulkan {

struct DeviceImpl;

struct MemoryImpl : public Memory::Impl {
  MemoryImpl(VmaAllocation allocation, void *data, DeviceImpl &device)
      : allocation(allocation), data(data), device(device) {}

  virtual Result<Memory::AccessMut> access() override;
  virtual Result<Memory::AccessConst> access() const override;

  inline VmaAllocation get_allocation() const { return this->allocation; }
  VmaAllocator get_allocator() const;

  virtual ~MemoryImpl() override;

private:
  VmaAllocation allocation;

  void *data;

  DeviceImpl &device;
};

struct MemoryAccessImpl : public Memory::AccessMut::Impl {
  MemoryAccessImpl(void *data, VmaAllocator allocator,
                   VmaAllocation allocation);

  virtual const std::byte *read() const override;
  virtual std::byte *write() override;

  virtual ~MemoryAccessImpl() override;

private:
  void *data;

  VmaAllocator allocator;
  VmaAllocation allocation;

  mutable bool needs_invalidate = true;
  bool needs_flush = false;
};

VmaAllocationCreateInfo
make_allocation_create_info(const Memory::Descriptor &desc);

} // namespace fgla::backends::vulkan
