#include <fgla/backends/vulkan/memory.hpp>

namespace fgla::backends::vulkan {

Result<Memory::AccessMut> MemoryImpl::access() {
  /* TODO */
}

Result<Memory::AccessConst> MemoryImpl::access() const {
  /* TODO */
}

const std::byte *MemoryAccessImpl::read() const {
  /* TODO */
}

std::byte *MemoryAccessImpl::write() {
  /* TODO */
}

} // namespace fgla::backends::vulkan
