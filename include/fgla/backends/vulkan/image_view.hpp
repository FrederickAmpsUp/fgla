#pragma once

#include <fgla/backends/vulkan/image.hpp>
#include <fgla/image_view.hpp>
#include <vulkan/vulkan.h>

namespace fgla::backends::vulkan {

struct ImageViewImpl : public ImageView::Impl {
  ImageViewImpl(BaseImageImpl &image, VkImageView image_view, VkExtent3D extent,
                VkDevice device)
      : image(image), image_view(image_view), extent(extent), device(device) {}

  inline BaseImageImpl &get_image() const { return this->image; }
  inline VkImageView get_view() const { return this->image_view; }

  inline const VkExtent3D &get_extent() const { return this->extent; }

  virtual inline ~ImageViewImpl() override {
    vkDestroyImageView(this->device, this->image_view, nullptr);
  }

private:
  BaseImageImpl &image;
  VkImageView image_view;
  VkExtent3D extent;
  VkDevice device;
};
} // namespace fgla::backends::vulkan
