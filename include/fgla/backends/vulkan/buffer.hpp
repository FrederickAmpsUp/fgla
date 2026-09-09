#pragma once

#include <fgla/buffer.hpp>
#include <utility>
#include <vulkan/vulkan.h>

namespace fgla::backends::vulkan {

struct BufferImpl : public Buffer::Impl {
  inline BufferImpl(VkBuffer buffer, Memory &&memory)
      : buffer(buffer), memory(std::move(memory)) {}

  virtual inline Memory &get_memory() override { return this->memory; }
  virtual inline const Memory &get_memory() const override {
    return this->memory;
  }

private:
  VkBuffer buffer;
  Memory memory;
};

} // namespace fgla::backends::vulkan
