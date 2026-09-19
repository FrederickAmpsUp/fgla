#include <fgla/backends/vulkan/device.hpp>
#include <fgla/backends/vulkan/image.hpp>
#include <fgla/backends/vulkan/memory.hpp>
#include <fgla/backends/vulkan/util.hpp>

namespace fgla::backends::vulkan {

VkImageAspectFlags format_aspect(VkFormat format) {
  switch (format) {
  case VK_FORMAT_D16_UNORM:
  case VK_FORMAT_X8_D24_UNORM_PACK32:
  case VK_FORMAT_D32_SFLOAT:
    return VK_IMAGE_ASPECT_DEPTH_BIT;

  case VK_FORMAT_S8_UINT:
    return VK_IMAGE_ASPECT_STENCIL_BIT;

  case VK_FORMAT_D16_UNORM_S8_UINT:
  case VK_FORMAT_D24_UNORM_S8_UINT:
  case VK_FORMAT_D32_SFLOAT_S8_UINT:
    return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;

  default:
    return VK_IMAGE_ASPECT_COLOR_BIT;
  }
}

Result<Image> DeviceImpl::create_image(const Image::Descriptor &desc) {
  VkImageCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;

  create_info.usage = vulkanize(desc.usage);

  create_info.imageType = vulkanize(desc.dimension);
  create_info.format = vulkanize(desc.format);

  create_info.extent = {desc.extent.x, desc.extent.y, desc.extent.z};
  create_info.arrayLayers = desc.array_layers;

  create_info.mipLevels = desc.mip_levels;

  create_info.samples = VK_SAMPLE_COUNT_1_BIT; // TODO
  create_info.tiling = VK_IMAGE_TILING_OPTIMAL;

  create_info.sharingMode = this->get_resource_sharing_mode();
  create_info.queueFamilyIndexCount = this->used_queue_family_indices.size();
  create_info.pQueueFamilyIndices = this->used_queue_family_indices.data();

  create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

  VmaAllocationCreateInfo allocation_create_info =
      make_allocation_create_info(desc.memory_properties);

  VkImage image;
  VmaAllocation allocation;
  VmaAllocationInfo allocation_info;

  VkResult res =
      vmaCreateImage(this->allocator, &create_info, &allocation_create_info,
                     &image, &allocation, &allocation_info);

  if (res != VK_SUCCESS) {
    return Error(1, "Failed to create Vulkan image");
  }

  Memory memory = Memory::from_impl(std::make_unique<MemoryImpl>(
      allocation, allocation_info.pMappedData, *this));

  return Image::from_impl(std::make_unique<OwnedImageImpl>(
      image, create_info.extent, format_aspect(create_info.format),
      std::move(memory), this->device));
}

} // namespace fgla::backends::vulkan
