#include <fgla/ext/windowing.hpp>
#include <fgla/instance.hpp>

int main(int argc, char **argv) {
  auto instance = fgla::unwrap(fgla::Instance::create({
                                   .app_version = {0, 0, 1},
                                   .app_name = "Windowing test",
                               }),
                               "Failed to create instance");

  auto &windowing =
      fgla::unwrap(instance.get_extension<fgla::ext::Windowing>(), "Windowing not available").get();

  auto window =
      fgla::unwrap(windowing.create_window({.width = 800, .height = 600, .name = "Test Window"}),
                   "Window creation failed");

  auto surface = fgla::unwrap(window.create_surface(instance), "Failed to create surface");

  while (window.is_open()) {
    window.poll_events();
  }
}
