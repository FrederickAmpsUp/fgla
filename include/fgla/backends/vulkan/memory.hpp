#pragma once

#include <fgla/memory.hpp>
#include <vulkan/vulkan.h>

namespace fgla::backends::vulkan {

struct DeviceImpl;

struct MemoryImpl : public Memory::Impl {
  MemoryImpl(VkDeviceMemory device_memory, VkDeviceSize offset,
             VkDeviceSize size, void *data, DeviceImpl &device)
      : device_memory(device_memory), offset(offset), size(size), data(data),
        device(device) {}

  virtual Result<Memory::AccessMut> access() override;
  virtual Result<Memory::AccessConst> access() const override;

  inline VkDeviceMemory get_device_memory() const {
    return this->device_memory;
  }
  inline VkDeviceSize get_offset() const { return this->offset; }
  inline VkDeviceSize get_size() const { return this->size; }

  virtual ~MemoryImpl() override;

private:
  VkDeviceMemory device_memory;
  VkDeviceSize offset, size;

  void *data;

  DeviceImpl &device;
};

struct MemoryAccessImpl : public Memory::AccessMut::Impl {
  MemoryAccessImpl(void *data, VkDevice device, VkDeviceMemory memory,
                   VkDeviceSize offset, VkDeviceSize size);

  virtual const std::byte *read() const override;
  virtual std::byte *write() override;

  virtual ~MemoryAccessImpl() override;

private:
  void *data;

  VkDevice device;
  VkMappedMemoryRange memory_range;

  mutable bool needs_invalidate = true;
  bool needs_flush = false;
};

} // namespace fgla::backends::vulkan
