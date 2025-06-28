#pragma once

#include <fgla/device.hpp>
#include <fgla/error.hpp>
#include <memory>
#include <tl/expected.hpp>

namespace fgla {

class Adapter {
public:
  struct Descriptor {
    // TODO
  };

  struct Info {
    std::string device_name;
  };

  inline tl::expected<Device, Error> create_device(const Device::Descriptor &descriptor,
                                                   std::initializer_list<Queue::Request> queues) {
    return this->impl->create_device(descriptor, queues);
  }

  inline Info get_info() const { return this->impl->get_info(); };

  struct Impl {
    virtual tl::expected<Device, Error> create_device(const Device::Descriptor &,
                                                      std::initializer_list<Queue::Request>) = 0;

    virtual Info get_info() const = 0;

    virtual ~Impl() = 0;
  };

  static inline Adapter from_raw(std::unique_ptr<Impl> impl) {
    Adapter adapter;
    adapter.impl = std::move(impl);
    return adapter;
  }

private:
  std::unique_ptr<Impl> impl;
};

inline Adapter::Impl::~Impl() = default;

} // namespace fgla
