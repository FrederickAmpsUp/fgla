#include <fgla/backends/vulkan/image.hpp>
#include <fgla/backends/vulkan/image_view.hpp>
#include <fgla/backends/vulkan/util.hpp>

namespace fgla::backends::vulkan {

Result<ImageView> BaseImageImpl::create_view(const ImageView::Descriptor &desc) {
  VkImageViewCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  create_info.image = this->image;

  create_info.viewType = vulkanize(desc.mode);
  create_info.format = vulkanize(desc.format);

  create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY; // likely can't expose this due to not
                                                            // being supported in other APIs

  create_info.subresourceRange.aspectMask =
      VK_IMAGE_ASPECT_COLOR_BIT; // TODO: expose this to the user
  create_info.subresourceRange.baseMipLevel = desc.base_mip_level;
  create_info.subresourceRange.levelCount = desc.num_mip_levels;
  create_info.subresourceRange.baseArrayLayer = desc.base_array_layer;
  create_info.subresourceRange.layerCount = desc.num_array_layers;

  VkImageView view;
  VkResult res = vkCreateImageView(this->device, &create_info, nullptr, &view);

  if (res != VK_SUCCESS) {
    return Error(1, "Failed to create Vulkan image view");
  }

  return ImageView::from_raw(std::make_unique<ImageViewImpl>(view, this->device));
}
} // namespace fgla::backends::vulkan
