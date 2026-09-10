#pragma once

#include <fgla/buffer.hpp>
#include <utility>
#include <vulkan/vulkan.h>

namespace fgla::backends::vulkan {

struct BufferImpl : public Buffer::Impl {
  inline BufferImpl(VkBuffer buffer, Memory &&memory, VkDevice device)
      : buffer(buffer), memory(std::move(memory)), device(device) {}

  virtual inline Memory &get_memory() override { return this->memory; }
  virtual inline const Memory &get_memory() const override {
    return this->memory;
  }

  inline VkBuffer get_buffer() const { return this->buffer; }

  virtual ~BufferImpl() override;

private:
  VkBuffer buffer;
  Memory memory;
  VkDevice device;
};

} // namespace fgla::backends::vulkan
