#pragma once

#include <cstdint>
#include <fgla/completion.hpp>
#include <vulkan/vulkan.h>

namespace fgla::backends::vulkan {

struct CompletionImpl : public fgla::Completion::Impl {
  CompletionImpl(VkSemaphore semaphore, uint64_t value)
      : semaphore(semaphore), value(value) {}

  virtual inline std::unique_ptr<fgla::Completion::Impl>
  clone() const override {
    return std::make_unique<CompletionImpl>(this->semaphore, this->value);
  }

  inline VkSemaphore get_semaphore() const { return this->semaphore; }
  inline uint64_t get_value() const { return this->value; }

  virtual ~CompletionImpl() override = default;

private:
  VkSemaphore semaphore;
  uint64_t value;
};

} // namespace fgla::backends::vulkan
