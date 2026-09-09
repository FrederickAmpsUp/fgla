#pragma once

#include <fgla/memory.hpp>
#include <vulkan/vulkan.h>

namespace fgla::backends::vulkan {

struct MemoryImpl : public Memory::Impl {
  virtual Result<Memory::AccessMut> access() override;
  virtual Result<Memory::AccessConst> access() const override;

private:
  VkDeviceMemory device_memory;
  VkDeviceSize offset, size;
};

struct MemoryAccessImpl : public Memory::AccessMut::Impl {
  const std::byte *read() const override;
  std::byte *write() override;
};

} // namespace fgla::backends::vulkan
