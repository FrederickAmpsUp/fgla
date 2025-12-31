#include <fgla/backends/vulkan/command_buffer.hpp>
#include <fgla/backends/vulkan/queue.hpp>
#include <spdlog/spdlog.h>

namespace fgla::backends::vulkan {

CommandBufferImpl::~CommandBufferImpl() {
  static auto logger = spdlog::get("fgla::backends::vulkan");
  if (this->is_recording) {
    logger->warn("Command buffer freed without being submitted.");
    vkEndCommandBuffer(this->command_buffer);
  }
}

} // namespace fgla::backends::vulkan
