#pragma once

#include <fgla/image.hpp>
#include <vulkan/vulkan.h>

namespace fgla::backends::vulkan {

struct BaseImageImpl : public Image::Impl {
  inline VkImage get_image() { return this->image; }

  virtual ~BaseImageImpl() = 0;
protected:
  BaseImageImpl(VkImage image) : image(image) {}
  
  VkImage image;
};

inline BaseImageImpl::~BaseImageImpl() = default;

struct InternalImageImpl : public BaseImageImpl {
  InternalImageImpl(VkImage image) : BaseImageImpl(image) {}

  virtual ~InternalImageImpl() override;
};

} // namespace fgla::backends::vulkan
