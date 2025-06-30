#include <fgla/ext/windowing/extension.hpp>
#include <fgla/instance.hpp>

int main(int argc, char **argv) {
  auto instance =
      fgla::unwrap_or_display_and_exit(fgla::Instance::create({{}}), "Failed to create instance");

  auto &windowing = fgla::unwrap_or_display_and_exit(
      instance.get_extension<fgla::ext::windowing::WindowExtension>(), "Windowing not available!");

  auto window = fgla::unwrap_or_display_and_exit(
      windowing.create_window({.width = 800, .height = 600, .name = "Test Window"}),
      "Window creation failed");

  while (window.is_open()) {
    window.poll_events();
  }
}
