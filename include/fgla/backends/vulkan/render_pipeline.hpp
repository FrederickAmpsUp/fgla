#pragma once

#include <fgla/render_pipeline.hpp>
#include <vulkan/vulkan.h>

namespace fgla::backends::vulkan {

struct RenderPipelineImpl : public RenderPipeline::Impl {
  RenderPipelineImpl(VkPipelineLayout pipeline_layout, VkPipeline pipeline, VkDevice device) :
    pipeline_layout(pipeline_layout), pipeline(pipeline), device(device) {}
 
  inline VkPipeline get_pipeline() const {
    return this->pipeline;
  }

  virtual ~RenderPipelineImpl() override;
private:
  VkPipelineLayout pipeline_layout;
  VkPipeline pipeline;
  VkDevice device;
};

} // namespace fgla::backends::vulkan
