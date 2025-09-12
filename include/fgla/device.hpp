#pragma once

#include <fgla/internal.hpp>
#include <fgla/queue.hpp>
#include <fgla/util.hpp>
#include <optional>
#include <memory>

namespace fgla {

/// Represents a device, which is used to create rendering objects
class Device {
public:
  /// Represents the settings used to create a `Device`
  struct Descriptor {};

  /// Retrieve a `Queue` of the given type and index
  ///
  /// @param type The type of queue to retrieve (e.g., `Queue::Type::Graphics` or
  /// `Queue::Type::Transfer`)
  /// @param index The index of the queue in the specified type
  /// @returns The specified queue, or `std::nullopt` if not found
  inline std::optional<std::reference_wrapper<Queue>> get_queue(Queue::Type type, uint32_t index) {
    Queue *q = this->impl->get_queue(type, index);
    if (q)
      return *q;
    else
      return {};
  }

  /// The backend-defined implementation of the `Device`'s functions
  struct Impl {
    virtual Queue *get_queue(Queue::Type, uint32_t) = 0;

    virtual ~Impl() = 0;
  };

  /// Creates a `Device` from a raw implementation
  /// This should only be used internally
  static inline Device from_raw(std::unique_ptr<Impl> impl) {
    Device device;
    device.impl = std::move(impl);
    return device;
  }

private:
  friend struct fgla::internal::ImplAccessor;
  std::unique_ptr<Impl> impl;
};

inline Device::Impl::~Impl() = default;
} // namespace fgla
