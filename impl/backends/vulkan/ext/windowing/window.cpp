#include <fgla/backends/vulkan/adapter.hpp>
#include <fgla/backends/vulkan/ext/windowing/extension.hpp>
#include <fgla/backends/vulkan/ext/windowing/surface.hpp>
#include <fgla/backends/vulkan/ext/windowing/window.hpp>
#include <set>
#include <spdlog/spdlog.h>
#include <string>

namespace fgla::backends::vulkan::ext::windowing {

Result<fgla::ext::windowing::Window> WindowingExtensionImpl::create_window(
    const fgla::ext::windowing::Window::Descriptor &descriptor) {
  std::unique_ptr<WindowImpl> impl = std::make_unique<WindowImpl>(descriptor);
  if (!impl->is_ok()) {
    return Error(0);
  }
  return fgla::ext::windowing::Window::from_impl(std::move(impl));
}

std::function<bool(const Adapter &)>
WindowingExtensionImpl::surface_support_filter(
    const fgla::ext::windowing::Surface &surface) {
  return [&](const Adapter &a) -> bool {
    auto &a_impl = a.to_impl<AdapterImpl>();
    auto &s_impl = surface.to_impl<SurfaceImpl>();

    uint32_t n_extensions = 0;
    vkEnumerateDeviceExtensionProperties(a_impl.get_physical_device(), nullptr,
                                         &n_extensions, nullptr);
    std::vector<VkExtensionProperties> extensions(n_extensions);
    vkEnumerateDeviceExtensionProperties(a_impl.get_physical_device(), nullptr,
                                         &n_extensions, extensions.data());

    std::set<std::string> required_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    for (const auto &extension : extensions) {
      required_extensions.erase(extension.extensionName);
    }

    if (!required_extensions.empty()) return false;

    uint32_t n_queue_families = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(a_impl.get_physical_device(),
                                             &n_queue_families, nullptr);

    VkBool32 present_support = false;
    for (int i = 0; n_queue_families; ++i) {
      vkGetPhysicalDeviceSurfaceSupportKHR(a_impl.get_physical_device(), i,
                                           s_impl.get_surface(),
                                           &present_support);

      if (present_support) break;
    }

    return present_support;
  };
}

WindowingExtensionImpl windowing_extension_impl = {};

WindowImpl::WindowImpl(
    const fgla::ext::windowing::Window::Descriptor &descriptor) {
  static auto logger = spdlog::get("fgla::backends::vulkan");
  if (!add_window()) {
    this->window = nullptr;
    return;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, descriptor.resizable);

  this->window = glfwCreateWindow(descriptor.width, descriptor.height,
                                  descriptor.name, nullptr, nullptr);

  logger->info("GLFW window created ({}x{}, \"{}\").", descriptor.width,
               descriptor.height, descriptor.name);
}

std::vector<fgla::ext::windowing::Event> WindowImpl::poll_events() {
  glfwPollEvents();

  using namespace fgla::ext::windowing;
  std::vector<Event> events;

  // mouse move event
  {
    double new_x_d, new_y_d;
    glfwGetCursorPos(window, &new_x_d, &new_y_d);
    float new_x = (float)new_x_d, new_y = (float)new_y_d;

    if (last_cursor_x != new_x || last_cursor_y != new_y) {
      if (last_cursor_x == -1e9f) last_cursor_x = new_x;
      if (last_cursor_y == -1e9f) last_cursor_y = new_y;
      events.push_back(MouseEvent{
        .old_x = last_cursor_x,
        .old_y = last_cursor_y,
        .new_x = new_x,
        .new_y = new_y,
        .delta_x = new_x - last_cursor_x,
        .delta_y = new_y - last_cursor_y
      });
    }
    last_cursor_x = new_x;
    last_cursor_y = new_y;
  }

  // key event
  {
    KeyEvent event{};
    for (auto keycode : ALL_KEY_CODES)
    {
      int key_state;
      switch (keycode)
      {
      case KeyCode::MouseLeftButton:
        key_state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        break;
      case KeyCode::MouseMiddleButton:
        key_state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);
        break;
      case KeyCode::MouseRightButton:
        key_state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
        break;
      default:
        key_state = glfwGetKey(window, (int)keycode);
      }

      if (key_state == GLFW_PRESS && !pressed_keys.count(keycode)) {
        event.pressed.push_back(keycode);
        pressed_keys.insert(keycode);
      } else if (key_state == GLFW_RELEASE && pressed_keys.count(keycode)) {
        event.released.push_back(keycode);
        pressed_keys.erase(keycode);
      }
    }

    if (!event.pressed.empty() || !event.released.empty()) {
      events.push_back(event);
    }
  }

  return events;
}

bool WindowImpl::is_open() { return !glfwWindowShouldClose(this->window); }

Result<fgla::ext::windowing::Surface>
WindowImpl::create_surface(const fgla::Instance &instance) {
  auto surface = std::make_unique<SurfaceImpl>(*this, instance);

  if (!surface->is_ok()) {
    return Error(0, "Failed to create Vulkan surface");
  }

  return fgla::ext::windowing::Surface::from_impl(std::move(surface));
}

Extent2d WindowImpl::get_framebuffer_size() const {
  int32_t width, height;
  glfwGetFramebufferSize(this->window, &width, &height);

  return {(uint32_t)width, (uint32_t)height};
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
