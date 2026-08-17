#include <fgla/backends/vulkan/shader_module.hpp>

namespace fgla::backends::vulkan {

ShaderModuleImpl::~ShaderModuleImpl() {
  vkDestroyShaderModule(this->device, this->shader_module, nullptr);
}

} // namespace fgla::backends::vulkan
