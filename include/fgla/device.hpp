#pragma once

#include <fgla/queue.hpp>
#include <memory>

namespace fgla {

class Device {
public:
  struct Descriptor {};

  inline Queue *get_queue(Queue::Type type, uint32_t index) {
    return this->impl->get_queue(type, index);
  }

  struct Impl {
    virtual Queue *get_queue(Queue::Type, uint32_t) = 0;

    virtual ~Impl() = 0;
  };

  static inline Device from_raw(std::unique_ptr<Impl> impl) {
    Device device;
    device.impl = std::move(impl);
    return device;
  }

private:
  std::unique_ptr<Impl> impl;
};

inline Device::Impl::~Impl() = default;
} // namespace fgla
