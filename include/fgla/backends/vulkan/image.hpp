#pragma once

#include <fgla/backends/vulkan/memory.hpp>
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
  inline const VkImageLayout &get_layout() const { return this->layout; }
  inline VkImageAspectFlags get_aspects() const { return this->aspect; }
  
  virtual ~BaseImageImpl() = 0;

protected:
  BaseImageImpl(VkImage image, VkExtent3D extent, VkImageAspectFlags aspect, VkDevice device)
      : image(image), device(device), extent(extent), aspect(aspect) {}

  Completion completion;

  VkImage image;
  VkDevice device;

  VkExtent3D extent = {0, 0, 0};
  VkImageAspectFlags aspect;

  VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
};

inline BaseImageImpl::~BaseImageImpl() = default;

struct OwnedImageImpl : public BaseImageImpl {
  OwnedImageImpl(VkImage image, VkExtent3D extent, VkImageAspectFlags aspect, Memory &&memory, VkDevice device)
      : memory(std::move(memory)), BaseImageImpl(image, extent, aspect, device) {}

  virtual ~OwnedImageImpl() override;

private:
  Memory memory;
};

} // namespace fgla::backends::vulkan
