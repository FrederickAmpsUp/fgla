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

  std::vector<VkBuffer> buffers;
  std::vector<VkDeviceSize> offsets;

  for (const auto &buffer : descriptor.vertex_buffers) {
    buffers.push_back(buffer.get().to_impl<BufferImpl>().get_buffer());
    offsets.push_back(0);
  }

  vkCmdBindVertexBuffers(this->command_buffer, 0, buffers.size(),
                         buffers.data(), offsets.data());

  vkCmdDraw(this->command_buffer, descriptor.vertex_count,
            descriptor.instance_count, descriptor.first_vertex,
            descriptor.first_instance);
}

RenderPassImpl::~RenderPassImpl() { vkCmdEndRendering(this->command_buffer); }

} // namespace fgla::backends::vulkan
