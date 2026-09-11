#include <fgla/backends/vulkan/completion.hpp>

namespace fgla::backends::vulkan {

fgla::Completion CompletionImpl::clone() const {
  return fgla::Completion::from_impl(std::make_unique<CompletionImpl>(
      this->device, this->semaphore, this->value));
}

void CompletionImpl::wait() const {
  VkSemaphoreWaitInfo wait_info = {};
  wait_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
  wait_info.semaphoreCount = 1;
  wait_info.pSemaphores = &this->semaphore;
  wait_info.pValues = &this->value;

  vkWaitSemaphores(this->device, &wait_info, UINT64_MAX);
}

} // namespace fgla::backends::vulkan
