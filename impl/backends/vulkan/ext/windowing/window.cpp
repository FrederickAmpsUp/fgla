#include <fgla/backends/vulkan/ext/windowing/extension.hpp>
#include <fgla/backends/vulkan/ext/windowing/surface.hpp>
#include <fgla/backends/vulkan/ext/windowing/window.hpp>
#include <fgla/backends/vulkan/adapter.hpp>
#include <fgla/internal.hpp>
#include <memory>
#include <spdlog/spdlog.h>


namespace fgla::backends::vulkan::ext::windowing {

tl::expected<fgla::ext::windowing::Window, Error>
WindowExtensionImpl::create_window(const fgla::ext::windowing::Window::Descriptor &descriptor) {
  std::unique_ptr<WindowImpl> impl = std::make_unique<WindowImpl>(descriptor);
  if (!impl->is_ok()) {
    return tl::make_unexpected(Error(0));
  }
  return fgla::ext::windowing::Window::from_raw(std::move(impl));
}

std::function<bool(const Adapter&)> WindowExtensionImpl::surface_support_filter(const fgla::ext::windowing::Surface &surface) {
  return [&](const Adapter &a) -> bool {
    auto a_impl = dynamic_cast<AdapterImpl *>(fgla::internal::ImplAccessor::get_impl(a));
    auto s_impl = dynamic_cast<SurfaceImpl *>(fgla::internal::ImplAccessor::get_impl(surface));
    
    uint32_t n_queue_families = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(a_impl->get_physical_device(), &n_queue_families, nullptr);
    
    VkBool32 present_support = false;
    for (int i = 0; n_queue_families; ++i) {
      vkGetPhysicalDeviceSurfaceSupportKHR(a_impl->get_physical_device(), i, s_impl->get_surface(), &present_support);
      
      if (present_support) break;
    }

    return present_support;
  };
}

WindowExtensionImpl window_extension_impl = {};

WindowImpl::WindowImpl(const fgla::ext::windowing::Window::Descriptor &descriptor) {
  static auto logger = spdlog::get("fgla::backends::vulkan");
  if (!add_window()) {
    this->window = nullptr;
    return;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  this->window =
      glfwCreateWindow(descriptor.width, descriptor.height, descriptor.name, nullptr, nullptr);

  logger->info("GLFW window created ({}x{}, \"{}\").", descriptor.width, descriptor.height,
               descriptor.name);
}

void WindowImpl::poll_events() { glfwPollEvents(); }

bool WindowImpl::is_open() { return !glfwWindowShouldClose(this->window); }

tl::expected<fgla::ext::windowing::Surface, fgla::Error>
WindowImpl::create_surface(const fgla::Instance &instance) {
  auto surface = std::make_shared<SurfaceImpl>(*this, instance);

  if (!surface->is_ok()) {
    return tl::make_unexpected(Error(0, "Failed to create Vulkan surface"));
  }

  return fgla::ext::windowing::Surface::from_raw(surface);
}

bool WindowImpl::is_ok() const { return this->window != nullptr; }

WindowImpl::~WindowImpl() {
  glfwDestroyWindow(this->window);
  remove_window();
}

bool init_windowing() { return glfwInit() == GLFW_TRUE; }

int *window_count() {
  static int c = 0;
  return &c;
}

bool add_window() {
  if (!init_windowing()) return false;
  ++(*window_count());
  return true;
}

void remove_window() {
  int &count = *window_count();
  if (count > 0) {
    --count;
    if (count == 0) glfwTerminate();
  }
}

} // namespace fgla::backends::vulkan::ext::windowing
