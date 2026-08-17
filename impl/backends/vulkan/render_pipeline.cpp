#include <fgla/backends/vulkan/render_pipeline.hpp>

namespace fgla::backends::vulkan {

RenderPipelineImpl::~RenderPipelineImpl() {
  vkDestroyPipeline(this->device, this->pipeline, nullptr);
  vkDestroyPipelineLayout(this->device, this->pipeline_layout, nullptr);
}

} // namespace fgla::backends::vulkan
