#pragma once

#include <fgla/shader_module.hpp>
#include <vulkan/vulkan.h>

namespace fgla::backends::vulkan {

struct ShaderModuleImpl : public ShaderModule::Impl {
  ShaderModuleImpl(VkShaderModule shader_module, VkDevice device)
      : shader_module(shader_module), device(device) {}

  inline VkShaderModule get_shader_module() const {
    return this->shader_module;
  }

  virtual ~ShaderModuleImpl() override;

private:
  VkShaderModule shader_module;

  VkDevice device;
};

} // namespace fgla::backends::vulkan
