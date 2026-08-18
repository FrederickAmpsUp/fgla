#include <fgla/backends/vulkan/render_pass.hpp>
#include <fgla/backends/vulkan/render_pipeline.hpp>

namespace fgla::backends::vulkan {

void RenderPassImpl::bind_pipeline(const RenderPipeline &pipeline) {
  VkPipeline vk_pipeline = dynamic_cast<RenderPipelineImpl *>(
                               fgla::internal::ImplAccessor::get_impl(pipeline))
                               ->get_pipeline();

  vkCmdBindPipeline(this->command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    vk_pipeline);
}

void RenderPassImpl::draw(uint32_t vertex_count) {
  vkCmdDraw(this->command_buffer, vertex_count, 1, 0, 0);
}

RenderPassImpl::~RenderPassImpl() { vkCmdEndRendering(this->command_buffer); }

} // namespace fgla::backends::vulkan
