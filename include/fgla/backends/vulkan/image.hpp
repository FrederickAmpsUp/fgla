#pragma once

#include <fgla/image.hpp>
#include <vulkan/vulkan.h>

namespace fgla::backends::vulkan {

struct BaseImageImpl : public Image::Impl {
  inline VkImage get_image() const { return this->image; }

  virtual Result<ImageView>
  create_view(const ImageView::Descriptor &desc) override;

  virtual inline Completion &get_completion() override {
    return this->completion;
  }

  inline VkImageLayout &get_layout() { return this->layout; }

  virtual ~BaseImageImpl() = 0;

protected:
  BaseImageImpl(VkImage image, VkDevice device)
      : image(image), device(device) {}

  Completion completion;

  VkImage image;
  VkDevice device;

  VkExtent3D extent = { 0, 0, 0 };

  VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
};

inline BaseImageImpl::~BaseImageImpl() = default;

// not really sure why this is here
struct InternalImageImpl : public BaseImageImpl {
  InternalImageImpl(VkImage image, VkDevice device)
      : BaseImageImpl(image, device) {}

  virtual ~InternalImageImpl() override;
};

} // namespace fgla::backends::vulkan
