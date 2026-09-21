#include <fgla/backends/vulkan/buffer.hpp>
#include <fgla/backends/vulkan/render_pass.hpp>
#include <fgla/backends/vulkan/render_pipeline.hpp>

namespace fgla::backends::vulkan {

// TODO: cache buffer bindings to avoid redundant bind calls
void RenderPassImpl::draw(const RenderPass::DrawDescriptor &descriptor) {
  VkPipeline vk_pipeline =
      descriptor.pipeline.to_impl<RenderPipelineImpl>().get_pipeline();

  if (vk_pipeline != this->current_pipeline) {
    vkCmdBindPipeline(this->command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      vk_pipeline);
    this->current_pipeline = vk_pipeline;
  }

  if (descriptor.vertex_buffers.size() > 0) {
    std::vector<VkBuffer> buffers;
    std::vector<VkDeviceSize> offsets;

    for (const auto &buffer : descriptor.vertex_buffers) {
      buffers.push_back(buffer.buffer.to_impl<BufferImpl>().get_buffer());
      offsets.push_back(buffer.offset);
    }

    vkCmdBindVertexBuffers(this->command_buffer, 0, buffers.size(),
                           buffers.data(), offsets.data());
  }

  if (descriptor.index_buffer) {
    VkIndexType index_type = VK_INDEX_TYPE_UINT16;
    if (descriptor.index_buffer->format ==
        fgla::RenderPass::DrawDescriptor::IndexBuffer::Format::UINT32)
      index_type = VK_INDEX_TYPE_UINT32;

    vkCmdBindIndexBuffer(
        this->command_buffer,
        descriptor.index_buffer->buffer.to_impl<BufferImpl>().get_buffer(),
        descriptor.index_buffer->offset, index_type);
  }

  if (descriptor.index_buffer)
    vkCmdDrawIndexed(this->command_buffer, descriptor.vertex_count,
                     descriptor.instance_count, descriptor.first_vertex, 0,
                     descriptor.first_instance);
  else
    vkCmdDraw(this->command_buffer, descriptor.vertex_count,
              descriptor.instance_count, descriptor.first_vertex,
              descriptor.first_instance);
}

RenderPassImpl::~RenderPassImpl() { vkCmdEndRendering(this->command_buffer); }

} // namespace fgla::backends::vulkan
