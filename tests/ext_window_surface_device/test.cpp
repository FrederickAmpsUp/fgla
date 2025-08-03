#include <fgla/ext/windowing/extension.hpp>
#include <fgla/instance.hpp>

int main(int argc, char **argv) {
  auto instance = fgla::unwrap_with_message(fgla::Instance::create({
      .required_extensions = {
        fgla::ext::windowing::WindowExtension::UUID
      },

      .app_version = {0, 0, 1},
      .app_name = "Windowing test",
    }),
  "Failed to create instance");

  auto &windowing = fgla::unwrap_with_message(
      instance.get_extension<fgla::ext::windowing::WindowExtension>(), "Windowing not available");

  auto window = fgla::unwrap_with_message(
      windowing.create_window({.width = 800, .height = 600, .name = "Test Window"}),
      "Window creation failed");

  auto surface =
      fgla::unwrap_with_message(window.create_surface(instance), "Failed to create surface");

  auto adapters = fgla::util::FilterableList(instance.enumerate_adapters());
  adapters = std::move(adapters).filter_move(windowing.surface_support_filter(surface));

  auto adapter = fgla::unwrap_with_message(instance.select_adapter(instance.get_adapter_scorer({}), adapters.data()),
                                           "Failed to create adapter");
  fgla::ext::windowing::PresentQueueOptions present_queue_opts = {
    surface, adapter,
  };
  auto device = fgla::unwrap_with_message(adapter.create_device({
  }, {
    fgla::Queue::Request{fgla::Queue::Type::Graphics, 1},
    fgla::Queue::Request{fgla::Queue::Type::Transfer, 1},
    fgla::Queue::Request{fgla::ext::windowing::QueueTypeExt::Present, 1, &present_queue_opts}
  }), "Failed to create device");

  fgla::Queue &graphics = *device.get_queue(fgla::Queue::Type::Graphics, 0);
  fgla::Queue &transfer = *device.get_queue(fgla::Queue::Type::Transfer, 0);
  fgla::Queue &present = *device.get_queue(fgla::ext::windowing::QueueTypeExt::Present, 0);

  while (window.is_open()) {
    window.poll_events();
  }
}
