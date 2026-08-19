#include <fgla/backends/vulkan/render_pass.hpp>
#include <fgla/backends/vulkan/render_pipeline.hpp>

namespace fgla::backends::vulkan {

void RenderPassImpl::draw(const RenderPipeline &pipeline,
                          uint32_t vertex_count) {
  VkPipeline vk_pipeline =
      pipeline.to_impl<RenderPipelineImpl>().get_pipeline();

  if (vk_pipeline != this->current_pipeline) {
    vkCmdBindPipeline(this->command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      vk_pipeline);
    this->current_pipeline = vk_pipeline;
  }

  vkCmdDraw(this->command_buffer, vertex_count, 1, 0, 0);
}

RenderPassImpl::~RenderPassImpl() { vkCmdEndRendering(this->command_buffer); }

} // namespace fgla::backends::vulkan
