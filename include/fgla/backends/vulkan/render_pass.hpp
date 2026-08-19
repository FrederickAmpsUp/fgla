#pragma once

#include <fgla/render_pass.hpp>
#include <vulkan/vulkan.h>

namespace fgla::backends::vulkan {

struct RenderPassImpl : public RenderPass::Impl {
  RenderPassImpl(VkCommandBuffer command_buffer)
      : command_buffer(command_buffer) {}

  virtual void draw(const RenderPipeline &pipeline,
                    uint32_t vertex_count) override;

  virtual ~RenderPassImpl() override;

private:
  VkCommandBuffer command_buffer;
  VkPipeline current_pipeline = VK_NULL_HANDLE;
};

} // namespace fgla::backends::vulkan
