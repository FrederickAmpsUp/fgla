#pragma once

#include <fgla/image_view.hpp>
#include <vulkan/vulkan.h>

namespace fgla::backends::vulkan {

struct ImageViewImpl : public ImageView::Impl {
  ImageViewImpl(VkImageView image_view, VkDevice device) : image_view(image_view), device(device) {}

  virtual inline ~ImageViewImpl() override {
    vkDestroyImageView(this->device, this->image_view, nullptr);
  }

private:
  VkImageView image_view;
  VkDevice device;
};
} // namespace fgla::backends::vulkan
