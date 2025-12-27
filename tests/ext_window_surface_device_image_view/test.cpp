#include <fgla/ext/windowing.hpp>
#include <fgla/fgla.hpp>
#include <magic_enum/magic_enum.hpp>
#include <spdlog/spdlog.h>

int main(int argc, char **argv) {
  auto instance =
      "Failed to create instance" * fgla::Instance::create({
                                        .required_extensions = {fgla::ext::Windowing::UUID},

                                        .app_version = {0, 0, 1},
                                        .app_name = "Windowing test",
                                    });

  auto &windowing =
      fgla::unwrap(instance.get_extension<fgla::ext::Windowing>(), "Windowing not available").get();

  auto window = "Window creation failed" *
                windowing.create_window({.width = 800, .height = 600, .name = "Test Window"});

  auto surface = "Surface creation failed" * window.create_surface(instance);

  auto adapters = fgla::util::FilterableList(instance.enumerate_adapters());
  adapters = std::move(adapters).filter_move(windowing.surface_support_filter(surface));

  auto adapter = "Failed to create adapter" *
                 instance.select_adapter(instance.get_adapter_scorer({}), adapters.data());
  fgla::ext::windowing::PresentQueueOptions present_queue_opts = {
      surface,
      adapter,
  };
  auto device =
      "Failed to create device" *
      adapter.create_device({}, {fgla::Queue::Request{fgla::Queue::Type::Graphics, 1},
                                 fgla::Queue::Request{fgla::Queue::Type::Transfer, 1},
                                 fgla::Queue::Request{fgla::ext::windowing::QueueTypeExt::Present,
                                                      1, &present_queue_opts}});

  fgla::Queue &graphics = *device.get_queue(fgla::Queue::Type::Graphics, 0);
  fgla::Queue &transfer = *device.get_queue(fgla::Queue::Type::Transfer, 0);
  fgla::Queue &present = *device.get_queue(fgla::ext::windowing::QueueTypeExt::Present, 0);

  auto caps = surface.get_capabilities(adapter);

  fgla::TextureFormat surface_format = fgla::TextureFormat::UNDEFINED;
  auto present_mode = fgla::ext::windowing::Surface::PresentMode::AUTO_VSYNC;

  spdlog::info(" ----- Format Support -----");
  for (const auto format : caps.formats) {
    spdlog::info("Supported format: {}", format.to_string());
    if (format == fgla::TextureFormat::B8G8R8A8_SRGB) surface_format = format;
  }

  if (surface_format == fgla::TextureFormat::UNDEFINED) {
    surface_format = caps.formats[0];
  }

  spdlog::info(" ----- Present Support -----");
  for (const auto present_mode : caps.present_modes) {
    spdlog::info("Supported present mode: {}", magic_enum::enum_name(present_mode));
  }

  auto fail =
      surface.configure(device, {surface_format, present_mode, window.get_framebuffer_size()});

  if (fail.has_value()) {
    spdlog::error("Surface configuration failed with message: \"{}\"",
                  (*fail).message.value_or(""));
    return 1;
  }

  while (window.is_open()) {
    auto &image = ("Failed to retrieve swapchain image" * surface.get_current_image(present)).get();
    auto image_view = image.create_view({
        .format = surface_format,
        .mode = fgla::ImageView::Mode::D2,
        .base_mip_level = 0,
        .num_mip_levels = 1,
        .base_array_layer = 0,
        .num_array_layers = 1,
    });

    // do stuff with it?
    window.poll_events();
  }
}
