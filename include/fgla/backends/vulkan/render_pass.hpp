#pragma once

#include <fgla/render_pass.hpp>
#include <vulkan/vulkan.h>

namespace fgla::backends::vulkan {

struct RenderPassImpl : public RenderPass::Impl {
  RenderPassImpl(VkCommandBuffer command_buffer)
      : command_buffer(command_buffer) {}

  virtual ~RenderPassImpl() override;

private:
  VkCommandBuffer command_buffer;
};

} // namespace fgla::backends::vulkan
