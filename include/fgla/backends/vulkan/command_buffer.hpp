#pragma once

#include <fgla/command_buffer.hpp>
#include <vulkan/vulkan.h>

namespace fgla::backends::vulkan {

struct QueueImpl;

struct CommandBufferImpl : public fgla::CommandBuffer::Impl {

  CommandBufferImpl(VkCommandBuffer command_buffer, VkFence fence)
      : command_buffer(command_buffer), fence(fence) {}
  inline VkCommandBuffer get_command_buffer() const { return this->command_buffer; }
  inline VkFence get_fence() const { return this->fence; }

  void end_recording();

  virtual ~CommandBufferImpl() override;

private:
  VkCommandBuffer command_buffer;
  VkFence fence;
  bool is_recording = true;
};

} // namespace fgla::backends::vulkan
