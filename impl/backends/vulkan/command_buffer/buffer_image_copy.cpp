#include "fgla/backends/vulkan/image.hpp"
#include <fgla/backends/vulkan/buffer.hpp>
#include <fgla/backends/vulkan/command_buffer.hpp>

namespace fgla::backends::vulkan {

void CommandBufferImpl::copy(
    const Buffer &src, const Buffer &dst,
    std::initializer_list<CommandBuffer::BufferCopy> regions) {
  std::vector<VkBufferCopy> buffer_copies;
  buffer_copies.reserve(regions.size());

  for (const auto &region : regions) {
    VkBufferCopy buffer_copy = {};
    buffer_copy.srcOffset = region.src_offset;
    buffer_copy.dstOffset = region.dst_offset;
    buffer_copy.size = region.size;

    buffer_copies.push_back(buffer_copy);
  }

  VkBuffer buf_src = src.to_impl<BufferImpl>().get_buffer();
  VkBuffer buf_dst = dst.to_impl<BufferImpl>().get_buffer();

  vkCmdCopyBuffer(this->command_buffer, buf_src, buf_dst, buffer_copies.size(),
                  buffer_copies.data());
}

std::vector<VkBufferImageCopy>
vulkanize(std::initializer_list<CommandBuffer::BufferImageCopy> regions) {
  std::vector<VkBufferImageCopy> buffer_image_copies;
  buffer_image_copies.reserve(regions.size());

  for (const auto &region : regions) {
    VkBufferImageCopy buffer_image_copy = {};
    buffer_image_copy.bufferOffset = region.buffer_offset;

    buffer_image_copy.bufferImageHeight = 0;
    buffer_image_copy.bufferRowLength = 0;

    // we use the same mapping as vulkan
    buffer_image_copy.imageSubresource.aspectMask =
        (VkImageAspectFlags)region.image_subresource.aspect_flags;
    buffer_image_copy.imageSubresource.baseArrayLayer =
        region.image_subresource.base_array_layer;
    buffer_image_copy.imageSubresource.layerCount =
        region.image_subresource.num_array_layers;
    buffer_image_copy.imageSubresource.mipLevel =
        region.image_subresource.base_mip_level;

    buffer_image_copy.imageOffset = {
        region.image_offset.x, region.image_offset.y, region.image_offset.z};
    buffer_image_copy.imageExtent = {
        region.image_extent.x, region.image_extent.y, region.image_extent.z};

    buffer_image_copies.push_back(buffer_image_copy);
  }

  return buffer_image_copies;
}

void CommandBufferImpl::copy(
    const Buffer &src, const Image &dst,
    std::initializer_list<CommandBuffer::BufferImageCopy> regions) {
  
  this->transition(const_cast<Image &>(dst), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

  std::vector<VkBufferImageCopy> buffer_image_copies = vulkanize(regions);

  VkBuffer buf_src = src.to_impl<BufferImpl>().get_buffer();
  VkImage img_dst = dst.to_impl<BaseImageImpl>().get_image();
  VkImageLayout layout_dst = dst.to_impl<BaseImageImpl>().get_layout();

  vkCmdCopyBufferToImage(this->command_buffer, buf_src, img_dst, layout_dst, buffer_image_copies.size(), buffer_image_copies.data());
}

void CommandBufferImpl::copy(
    const Image &src, const Buffer &dst,
    std::initializer_list<CommandBuffer::BufferImageCopy> regions) {

  this->transition(const_cast<Image &>(src), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

  std::vector<VkBufferImageCopy> buffer_image_copies = vulkanize(regions);

  VkImage img_src = src.to_impl<BaseImageImpl>().get_image();
  VkImageLayout layout_src = src.to_impl<BaseImageImpl>().get_layout();
  VkBuffer buf_dst = dst.to_impl<BufferImpl>().get_buffer();

  vkCmdCopyImageToBuffer(this->command_buffer, img_src, layout_src, buf_dst, buffer_image_copies.size(), buffer_image_copies.data());
}
} // namespace fgla::backends::vulkan
