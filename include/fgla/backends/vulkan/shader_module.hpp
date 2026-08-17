#pragma once

#include <fgla/shader_module.hpp>
#include <vulkan/vulkan.h>

namespace fgla::backends::vulkan {

struct ShaderModuleImpl : public ShaderModule::Impl {
  ShaderModuleImpl(VkShaderModule shader_module, VkDevice device)
    : shader_module(shader_module), device(device) {}

  virtual ~ShaderModuleImpl() override;
private:
  VkShaderModule shader_module;

  VkDevice device;
};

} // fgla::backends::vulkan
