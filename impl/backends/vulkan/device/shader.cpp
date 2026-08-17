#include <fgla/backends/vulkan/shader_module.hpp>
#include <fgla/backends/vulkan/device.hpp>

namespace fgla::backends::vulkan {

void DeviceImpl::init_shader(const std::vector<std::filesystem::path> &shader_paths) {
  SlangGlobalSessionDesc global_session_desc = {};
  slang::createGlobalSession(&global_session_desc, this->slang_global_session.writeRef());

  slang::SessionDesc session_desc = {};

  std::vector<const char *> search_paths;
  search_paths.reserve(shader_paths.size());

  for (const auto &path : shader_paths) {
    search_paths.push_back(path.c_str());
  }

  session_desc.searchPaths = search_paths.data();
  session_desc.searchPathCount = search_paths.size();

  slang::TargetDesc target_desc = {};
  target_desc.format = SLANG_SPIRV;
  target_desc.profile = this->slang_global_session->findProfile("spirv_1_5");

  session_desc.targets = &target_desc;
  session_desc.targetCount = 1;

  slang::PreprocessorMacroDesc fgla_backend = { "FGLA_BACKEND_VULKAN", "1" };
  session_desc.preprocessorMacros = &fgla_backend;
  session_desc.preprocessorMacroCount = 1;

  this->slang_global_session->createSession(session_desc, this->slang_session.writeRef());
}

Result<ShaderModule> DeviceImpl::load_shader_module(const ShaderModule::Descriptor &desc) {
  Slang::ComPtr<slang::IBlob> diagnostics;
  auto module = Slang::ComPtr<slang::IModule>(
    this->slang_session->loadModule(desc.module_name.c_str(), diagnostics.writeRef())
  );

  if (!module) {
    return Error(1, std::string((const char *)diagnostics->getBufferPointer()));
  }

  Slang::ComPtr<slang::IComponentType> linked_program;

  SlangResult link_result = module->link(linked_program.writeRef(), diagnostics.writeRef());

  if (SLANG_FAILED(link_result)) {
    return Error(2, std::string((const char *)diagnostics->getBufferPointer()));
  }

  Slang::ComPtr<slang::IBlob> spirv_code;
  SlangResult code_result = linked_program->getTargetCode(0, spirv_code.writeRef(), diagnostics.writeRef());

  if (SLANG_FAILED(code_result)) {
    return Error(3, std::string((const char *)diagnostics->getBufferPointer()));
  }

  VkShaderModule shader_module;

  VkShaderModuleCreateInfo shader_module_info = {};
  shader_module_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  shader_module_info.codeSize = spirv_code->getBufferSize();
  shader_module_info.pCode = (const uint32_t *)spirv_code->getBufferPointer();

  VkResult shader_module_result = vkCreateShaderModule(
    this->device,
    &shader_module_info,
    nullptr, &shader_module
  );

  if (shader_module_result != VK_SUCCESS) {
    return Error(4, "Failed to create Vulkan shader module");
  }

  return ShaderModule::from_raw(std::make_unique<ShaderModuleImpl>(shader_module, this->device));
}

} // namespace fgla::backends::vulkan
