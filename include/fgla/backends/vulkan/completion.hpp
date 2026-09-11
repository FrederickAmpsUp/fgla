#pragma once

#include <cstdint>
#include <fgla/completion.hpp>
#include <vulkan/vulkan.h>

namespace fgla::backends::vulkan {

struct CompletionImpl : public fgla::Completion::Impl {
  CompletionImpl(VkDevice device, VkSemaphore semaphore, uint64_t value)
      : device(device), semaphore(semaphore), value(value) {}

  virtual fgla::Completion clone() const override;

  virtual inline void wait() const override;

  inline VkSemaphore get_semaphore() const { return this->semaphore; }
  inline uint64_t get_value() const { return this->value; }

  virtual ~CompletionImpl() override = default;

private:
  VkDevice device;
  VkSemaphore semaphore;
  uint64_t value;
};

} // namespace fgla::backends::vulkan
