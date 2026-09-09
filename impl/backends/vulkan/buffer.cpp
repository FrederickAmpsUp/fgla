#include <fgla/backends/vulkan/buffer.hpp>

namespace fgla::backends::vulkan {

BufferImpl::~BufferImpl() {
  vkDestroyBuffer(this->device, this->buffer, nullptr);
}

} // namespace fgla::backends::vulkan
