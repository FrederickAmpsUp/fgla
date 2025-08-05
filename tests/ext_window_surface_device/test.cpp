#include <fgla/ext/windowing/extension.hpp>
#include <fgla/instance.hpp>

int main(int argc, char **argv) {
  auto instance = "Failed to create instance" *fgla::Instance::create({
                                   .required_extensions = {fgla::ext::Windowing::UUID},

                                   .app_version = {0, 0, 1},
                                   .app_name = "Windowing test",
                               });

  auto &windowing =
      fgla::unwrap(instance.get_extension<fgla::ext::Windowing>(), "Windowing not available");

  auto window =
      "Window creation failed" *windowing.create_window({.width = 800, .height = 600, .name = "Test Window"});

  auto surface = "Surface creation failed" *window.create_surface(instance);

  auto adapters = fgla::util::FilterableList(instance.enumerate_adapters());
  adapters = std::move(adapters).filter_move(windowing.surface_support_filter(surface));

  auto adapter =
      "Failed to create adapter" *instance.select_adapter(instance.get_adapter_scorer({}), adapters.data());
  fgla::ext::windowing::PresentQueueOptions present_queue_opts = {
      surface,
      adapter,
  };
  auto device = "Failed to create device"
      *adapter.create_device({}, {fgla::Queue::Request{fgla::Queue::Type::Graphics, 1},
                                 fgla::Queue::Request{fgla::Queue::Type::Transfer, 1},
                                 fgla::Queue::Request{fgla::ext::windowing::QueueTypeExt::Present,
                                                      1, &present_queue_opts}});

  fgla::Queue &graphics = *device.get_queue(fgla::Queue::Type::Graphics, 0);
  fgla::Queue &transfer = *device.get_queue(fgla::Queue::Type::Transfer, 0);
  fgla::Queue &present = *device.get_queue(fgla::ext::windowing::QueueTypeExt::Present, 0);

  while (window.is_open()) {
    window.poll_events();
  }
}
