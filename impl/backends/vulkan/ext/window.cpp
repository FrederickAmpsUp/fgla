#include <fgla/backends/vulkan/ext/window.hpp>
#include <spdlog/spdlog.h>

namespace fgla::backends::vulkan::ext::windowing {

fgla::ext::windowing::Window
WindowExtensionImpl::create_window(const fgla::ext::windowing::Window::Descriptor &descriptor) {
  std::unique_ptr<WindowImpl> impl = std::make_unique<WindowImpl>(descriptor);

  return fgla::ext::windowing::Window::from_raw(std::move(impl));
}

WindowExtensionImpl window_extension_impl = {};

WindowImpl::WindowImpl(const fgla::ext::windowing::Window::Descriptor &) {
  spdlog::info("created window!");
}
} // namespace fgla::backends::vulkan::ext::windowing
