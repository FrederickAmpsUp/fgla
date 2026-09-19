#include <fgla/backends/vulkan/image.hpp>
#include <fgla/backends/vulkan/image_view.hpp>
#include <fgla/backends/vulkan/util.hpp>

namespace fgla::backends::vulkan {

Result<ImageView>
BaseImageImpl::create_view(const ImageView::Descriptor &desc) {
  static auto logger = spdlog::get("fgla::backends::vulkan");

  VkImageViewCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  create_info.image = this->image;

  create_info.viewType = vulkanize(desc.mode);
  create_info.format = vulkanize(desc.format);

  create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  create_info.components.a =
      VK_COMPONENT_SWIZZLE_IDENTITY; // likely can't expose this due to not
                                     // being supported in other APIs

  create_info.subresourceRange.aspectMask =
      (VkImageAspectFlags)desc.subresource_range.aspect_flags;
  // this is ok because the fgla flags share the same
  // values as the vulkan ones
  create_info.subresourceRange.baseMipLevel =
      desc.subresource_range.base_mip_level;
  create_info.subresourceRange.levelCount =
      desc.subresource_range.num_mip_levels;
  create_info.subresourceRange.baseArrayLayer =
      desc.subresource_range.base_array_layer;
  create_info.subresourceRange.layerCount =
      desc.subresource_range.num_array_layers;

  VkImageView view;
  VkResult res = vkCreateImageView(this->device, &create_info, nullptr, &view);

  if (res != VK_SUCCESS) {
    return Error(1, "Failed to create Vulkan image view");
  }

  logger->info("Vulkan image view created.");

  VkExtent3D view_extent = {
      std::max(1u, this->extent.width >> desc.subresource_range.base_mip_level),
      std::max(1u,
               this->extent.height >> desc.subresource_range.base_mip_level),
      std::max(1u,
               this->extent.depth >> desc.subresource_range.base_mip_level)};

  return ImageView::from_impl(
      std::make_unique<ImageViewImpl>(*this, view, view_extent, this->device));
}

OwnedImageImpl::~OwnedImageImpl() {
  const MemoryImpl &mem = this->memory.to_impl<MemoryImpl>();

  vmaDestroyImage(mem.get_allocator(), this->image, mem.get_allocation());
}
} // namespace fgla::backends::vulkan
