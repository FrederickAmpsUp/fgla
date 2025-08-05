#pragma once

#include <fgla/device.hpp>
#include <fgla/error.hpp>
#include <fgla/internal.hpp>
#include <memory>

namespace fgla {

/// Represents an adapter, which is used to set up and create a `Device`
class Adapter {
public:
  /// Represents the settings used to create an `Adapter`
  struct Descriptor {
    // TODO
  };

  /// Represents information about an `Adapter`
  struct Info {
    std::string device_name;
  };

  /// Attempts to create a `Device`
  /// @param descriptor The `Device::Descriptor` with the `Device`'s properties
  /// @param queues A list of `Queue::Requests` to create `Queue`s for
  /// @returns The created `Device`, or an `Error` with failure information
  inline Result<Device> create_device(const Device::Descriptor &descriptor,
                                      std::initializer_list<Queue::Request> queues) {
    return this->impl->create_device(descriptor, queues);
  }

  /// Retrieves this `Adapter`'s `Info`
  inline Info get_info() const { return this->impl->get_info(); };

  /// The backend-defined implementation of the `Adapter`'s functions
  struct Impl {
    virtual Result<Device> create_device(const Device::Descriptor &,
                                         std::initializer_list<Queue::Request>) = 0;

    virtual Info get_info() const = 0;

    virtual ~Impl() = 0;
  };

  /// Creates an `Adapter` from a raw implementation
  /// This should only be used internally
  static inline Adapter from_raw(std::unique_ptr<Impl> impl) {
    Adapter adapter;
    adapter.impl = std::move(impl);
    return adapter;
  }

private:
  friend struct fgla::internal::ImplAccessor;
  std::unique_ptr<Impl> impl;
};

inline Adapter::Impl::~Impl() = default;

} // namespace fgla
