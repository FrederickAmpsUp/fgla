#include <cstdint>
#include <fgla/backends/vulkan/command_buffer.hpp>
#include <fgla/backends/vulkan/image_view.hpp>
#include <fgla/backends/vulkan/queue.hpp>
#include <fgla/backends/vulkan/render_pass.hpp>
#include <fgla/backends/vulkan/util.hpp>
#include <spdlog/spdlog.h>

namespace fgla::backends::vulkan {

Result<fgla::RenderPass>
CommandBufferImpl::begin_render_pass(const fgla::RenderPass::Descriptor &desc) {
  std::vector<VkRenderingAttachmentInfo> color_attachments(
      desc.color_attachments.size());
  std::vector<VkImageMemoryBarrier2> memory_barriers;

  for (uint32_t i = 0; i < desc.color_attachments.size(); ++i) {
    const auto &attachment_desc = desc.color_attachments[i];
    auto &attachment_info = color_attachments[i];

    const ImageViewImpl &view = *dynamic_cast<ImageViewImpl *>(
        fgla::internal::ImplAccessor::get_impl(attachment_desc.view));

    attachment_info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    attachment_info.imageView = view.get_view();
    attachment_info.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachment_info.loadOp = vulkanize(attachment_desc.load_op.op);
    attachment_info.clearValue.color = {attachment_desc.load_op.clear_color.r,
                                        attachment_desc.load_op.clear_color.g,
                                        attachment_desc.load_op.clear_color.b,
                                        attachment_desc.load_op.clear_color.a};

    attachment_info.storeOp = vulkanize(attachment_desc.store_op.op);

    if (view.get_image().get_layout() !=
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
      VkImageMemoryBarrier2 memory_barrier = {};
      memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;

      memory_barrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
      memory_barrier.srcAccessMask =
          VK_ACCESS_2_MEMORY_READ_BIT | VK_ACCESS_2_MEMORY_WRITE_BIT;

      memory_barrier.dstStageMask =
          VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
      memory_barrier.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;

      memory_barrier.oldLayout = view.get_image().get_layout();
      memory_barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

      memory_barrier.image = view.get_image().get_image();
      // TODO: change this to use the subresourceRange used to create view
      memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      memory_barrier.subresourceRange.baseMipLevel = 0;
      memory_barrier.subresourceRange.levelCount = 1;
      memory_barrier.subresourceRange.baseArrayLayer = 0;
      memory_barrier.subresourceRange.layerCount = 1;

      view.get_image().get_layout() = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

      memory_barriers.push_back(memory_barrier);
    }
  }

  // Transition the color attachments to the correct
  // layout before starting the render pass
  if (memory_barriers.size() > 0) {
    VkDependencyInfo dependency_info = {};
    dependency_info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dependency_info.imageMemoryBarrierCount = memory_barriers.size();
    dependency_info.pImageMemoryBarriers = memory_barriers.data();

    vkCmdPipelineBarrier2(this->command_buffer, &dependency_info);
  }

  VkRenderingInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;

  info.renderArea.offset = {0, 0};

  /// 🚨🚨🚨 WAR CRIME 🚨🚨🚨
  //  --- FIX THIS PLEASE ---
  info.renderArea.extent = {800, 600};

  info.layerCount = 1;

  info.colorAttachmentCount = color_attachments.size();
  info.pColorAttachments = color_attachments.data();

  info.pDepthAttachment = nullptr;
  info.pStencilAttachment = nullptr;

  vkCmdBeginRendering(this->command_buffer, &info);

  return RenderPass::from_raw(
      std::make_unique<RenderPassImpl>(this->command_buffer));
}

void CommandBufferImpl::end_recording() {
  if (!this->is_recording) return;
  this->is_recording = false;
  vkEndCommandBuffer(this->command_buffer);
}

CommandBufferImpl::~CommandBufferImpl() {
  static auto logger = spdlog::get("fgla::backends::vulkan");
  if (this->is_recording) logger->warn("Command buffer was never submitted.");
  this->end_recording();
}

} // namespace fgla::backends::vulkan
