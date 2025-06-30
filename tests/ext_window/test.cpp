#include <fgla/ext/windowing/extension.hpp>
#include <fgla/instance.hpp>
#include <iostream>

int main(int argc, char **argv) {
  tl::expected<fgla::Instance, fgla::Error> instance_res = fgla::Instance::create({{}});

  if (!instance_res) {
    std::cerr << "Failed to create instance!" << std::endl;
    return -1;
  }

  fgla::Instance instance = std::move(*instance_res);

  auto windowing_res = instance.get_extension<fgla::ext::windowing::WindowExtension>();

  if (!windowing_res) {
    std::cerr << "Windowing not supported!" << std::endl;
    return -1;
  }

  auto &windowing = *windowing_res;

  tl::expected<fgla::ext::windowing::Window, fgla::Error> window_res =
      windowing.create_window({.width = 800, .height = 600, .name = "Test Window"});
}
