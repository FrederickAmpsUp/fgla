#include <fgla/backends/vulkan/buffer.hpp>
#include <fgla/backends/vulkan/memory.hpp>

namespace fgla::backends::vulkan {

BufferImpl::~BufferImpl() {
  const MemoryImpl &mem = this->memory.to_impl<MemoryImpl>();

  vmaDestroyBuffer(mem.get_allocator(), this->buffer, mem.get_allocation());
}

} // namespace fgla::backends::vulkan
