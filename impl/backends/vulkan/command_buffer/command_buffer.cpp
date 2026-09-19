#include <fgla/backends/vulkan/command_buffer.hpp>
#include <fgla/backends/vulkan/queue.hpp>
#include <spdlog/spdlog.h>

namespace fgla::backends::vulkan {

void CommandBufferImpl::end_recording() {
  if (!this->is_recording) return;
  this->is_recording = false;
  vkEndCommandBuffer(this->command_buffer);
}

CommandBufferImpl::~CommandBufferImpl() {
  static auto logger = spdlog::get("fgla::backends::vulkan");
  if (this->is_recording) logger->warn("Command buffer was never submitted.");
  this->end_recording();
}

} // namespace fgla::backends::vulkan
