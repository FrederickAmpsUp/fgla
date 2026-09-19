#include <fgla/backends/vulkan/command_buffer.hpp>
#include <fgla/backends/vulkan/image.hpp>
#include <vulkan/vulkan_core.h>

namespace fgla::backends::vulkan {

VkAccessFlags2 layout_access_mask(VkImageLayout layout) {
  switch (layout) {
  case VK_IMAGE_LAYOUT_UNDEFINED:
    return 0;

  case VK_IMAGE_LAYOUT_GENERAL:
    return VK_ACCESS_2_SHADER_READ_BIT | VK_ACCESS_2_SHADER_WRITE_BIT |
           VK_ACCESS_2_TRANSFER_READ_BIT | VK_ACCESS_2_TRANSFER_WRITE_BIT |
           VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT |
           VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT |
           VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
           VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

  case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
    return VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT |
           VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;

  case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL:
  case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL:
    return VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
           (layout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL
                ? VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
                : 0);

  case VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL:
  case VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL:
    return VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
           (layout == VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL
                ? VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
                : 0);

  case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
    return VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
           VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

  case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
    return VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT;

  case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
    return VK_ACCESS_2_SHADER_READ_BIT;

  case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
    return VK_ACCESS_2_TRANSFER_READ_BIT;

  case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
    return VK_ACCESS_2_TRANSFER_WRITE_BIT;

  case VK_IMAGE_LAYOUT_PREINITIALIZED:
    return VK_ACCESS_2_HOST_WRITE_BIT;

  case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
    return VK_ACCESS_2_NONE;

  default:
    return VK_ACCESS_2_MEMORY_READ_BIT | VK_ACCESS_2_MEMORY_WRITE_BIT;
  }
}

void CommandBufferImpl::transition(Image &img, VkImageLayout target) {
  auto &img_impl = img.to_impl<BaseImageImpl>();

  if (img_impl.get_layout() == target) return;

  VkImageMemoryBarrier2 memory_barrier = {};
  memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;

  memory_barrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
  memory_barrier.srcAccessMask = layout_access_mask(img_impl.get_layout());

  memory_barrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
  memory_barrier.dstAccessMask = layout_access_mask(target);

  memory_barrier.oldLayout = img_impl.get_layout();
  memory_barrier.newLayout = target;

  memory_barrier.image = img_impl.get_image();

  memory_barrier.subresourceRange.aspectMask = img_impl.get_aspects();
  memory_barrier.subresourceRange.baseMipLevel = 0;
  memory_barrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
  memory_barrier.subresourceRange.baseArrayLayer = 0;
  memory_barrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

  VkDependencyInfo dependency_info = {};
  dependency_info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
  dependency_info.imageMemoryBarrierCount = 1;
  dependency_info.pImageMemoryBarriers = &memory_barrier;

  vkCmdPipelineBarrier2(this->command_buffer, &dependency_info);

  img_impl.get_layout() = target;
}

} // namespace fgla::backends::vulkan
