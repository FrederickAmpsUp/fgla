#if FGLA_VK_EXT_WINDOWING
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#endif

#include <fgla/backends/vulkan/adapter.hpp>
#include <fgla/backends/vulkan/backend.hpp>
#include <fgla/backends/vulkan/ext/windowing/extension.hpp>
#include <fgla/backends/vulkan/instance.hpp>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace fgla::backends::vulkan {

InstanceImpl::InstanceImpl(const Instance::Descriptor &descriptor) {
  static auto logger = spdlog::stdout_color_mt("fgla::backends::vulkan");
  logger->set_level(spdlog::level::debug); // TODO: modify this

  VkApplicationInfo app_info = {};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

  app_info.applicationVersion = VK_MAKE_VERSION(
      descriptor.app_version.major, descriptor.app_version.minor, descriptor.app_version.patch);
  app_info.pApplicationName = descriptor.app_name.c_str();

  logger->info("Creating FGLA Vulkan instance for app \"{}\" (v{}.{}.{}).",
               descriptor.app_name, descriptor.app_version.major, descriptor.app_version.minor, descriptor.app_version.patch);

  app_info.pEngineName = "FGLA";
  app_info.engineVersion = VK_MAKE_VERSION(0, 0, 1);

  app_info.apiVersion = VK_API_VERSION_1_3;

  VkInstanceCreateInfo instance_info = {};
  instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

  instance_info.pApplicationInfo = &app_info;

  std::vector<const char *> extensions;

#if FGLA_VK_EXT_WINDOWING
  uint32_t n_glfw_extensions = 0;
  const char **glfw_extensions;
  
  glfwInit();
  glfw_extensions = glfwGetRequiredInstanceExtensions(&n_glfw_extensions);

  extensions.insert(extensions.end(), glfw_extensions, glfw_extensions + n_glfw_extensions);
#endif

  instance_info.enabledExtensionCount = extensions.size();
  instance_info.ppEnabledExtensionNames = extensions.data();

  instance_info.enabledLayerCount = 0;

  VkResult result = vkCreateInstance(&instance_info, nullptr, &this->instance);
  if (result != VK_SUCCESS) {
    logger->error("Vulkan instance creation failed, error code {:X}", (unsigned int)result);
    this->instance = VK_NULL_HANDLE;
    return;
  }

  logger->info("Vulkan instance created.");
}
bool InstanceImpl::is_ok() const { return this->instance != VK_NULL_HANDLE; }

tl::expected<Adapter, Error> InstanceImpl::get_adapter(const Adapter::Descriptor &descriptor) {
  std::unique_ptr<AdapterImpl> impl =
      std::make_unique<AdapterImpl>(*this, descriptor); // may have to change this
  if (!impl->is_ok()) return tl::make_unexpected(Error(0));
  return std::move(Adapter::from_raw(std::move(impl)));
}

const backend::Backend &InstanceImpl::get_backend() {
  return fgla::backend::get_registry().at(UUID);
}

void *InstanceImpl::get_extension(extension::ExtensionUUID uuid) {
#ifdef FGLA_VK_EXT_WINDOWING
  if (uuid == fgla::ext::windowing::WindowExtension::UUID) {
    fgla::ext::windowing::WindowExtension &ext = ext::windowing::window_extension_impl;
    return static_cast<void *>(&ext);
  }
#endif

  return nullptr;
}

InstanceImpl::~InstanceImpl() { vkDestroyInstance(this->instance, nullptr); }

} // namespace fgla::backends::vulkan
