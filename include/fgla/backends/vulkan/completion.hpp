#pragma once

#include <cstdint>
#include <fgla/completion.hpp>
#include <vulkan/vulkan.h>

namespace fgla::backends::vulkan {

struct CompletionImpl : public fgla::Completion::Impl {
  CompletionImpl(VkDevice device, VkSemaphore semaphore, uint64_t value)
      : device(device), semaphore(semaphore), value(value) {}

  virtual inline fgla::Completion clone() const override {
    return fgla::Completion::from_impl(std::make_unique<CompletionImpl>(
        this->device, this->semaphore, this->value));
  }

  virtual inline void wait() const override {
    VkSemaphoreWaitInfo wait_info = {};
    wait_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
    wait_info.semaphoreCount = 1;
    wait_info.pSemaphores = &this->semaphore;
    wait_info.pValues = &this->value;

    vkWaitSemaphores(this->device, &wait_info, UINT64_MAX);
  }

  inline VkSemaphore get_semaphore() const { return this->semaphore; }
  inline uint64_t get_value() const { return this->value; }

  virtual ~CompletionImpl() override = default;

private:
  VkDevice device;
  VkSemaphore semaphore;
  uint64_t value;
};

} // namespace fgla::backends::vulkan
