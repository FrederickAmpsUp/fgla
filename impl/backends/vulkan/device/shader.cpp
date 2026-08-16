#include <fgla/backends/vulkan/device.hpp>

namespace fgla::backends::vulkan {

void DeviceImpl::init_shader() {
  SlangGlobalSessionDesc global_session_desc = {};
  slang::createGlobalSession(&global_session_desc, this->slang_global_session.writeRef());

  slang::SessionDesc session_desc = {};

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

} // namespace fgla::backends::vulkan
