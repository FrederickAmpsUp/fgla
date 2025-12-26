#pragma once

#include <fgla/image.hpp>
#include <vulkan/vulkan.h>

namespace fgla::backends::vulkan {

struct BaseImageImpl : public Image::Impl {
  inline VkImage get_image() { return this->image; }

  virtual Result<ImageView> create_view(const ImageView::Descriptor &desc) override;

  virtual ~BaseImageImpl() = 0;

protected:
  BaseImageImpl(VkImage image, VkDevice device) : image(image), device(device) {}

  VkImage image;
  VkDevice device;
};

inline BaseImageImpl::~BaseImageImpl() = default;

struct InternalImageImpl : public BaseImageImpl {
  InternalImageImpl(VkImage image) : BaseImageImpl(image) {}

  virtual ~InternalImageImpl() override;
};

} // namespace fgla::backends::vulkan
