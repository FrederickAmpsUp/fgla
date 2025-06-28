#pragma once

#include <cstdint>
#include <fgla/adapter.hpp>
#include <fgla/backends/vulkan/instance.hpp>
#include <optional>
#include <vulkan/vulkan.h>

namespace fgla::backends::vulkan {

struct QueueFamilyIndices {
  std::optional<uint32_t> graphics_family;

  static QueueFamilyIndices find(VkPhysicalDevice device);
};

struct AdapterImpl : public fgla::Adapter::Impl {
  AdapterImpl(const InstanceImpl &instance, const Adapter::Descriptor &descriptor);
  bool is_ok() const;

  inline VkPhysicalDevice get_physical_device() { return this->physical_device; }

  virtual Adapter::Info get_info() const override;
  virtual tl::expected<Device, Error>
  create_device(const Device::Descriptor &descriptor,
                std::initializer_list<Queue::Request> queue_requests) override;
  virtual ~AdapterImpl() override = default;

private:
  VkPhysicalDevice physical_device;
};

} // namespace fgla::backends::vulkan
