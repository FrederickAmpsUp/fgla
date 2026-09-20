#include <fgla/ext/windowing.hpp>
#include <fgla/fgla.hpp>
#include <fgla/render_pass.hpp>
#include <fgla/render_pipeline.hpp>
#include <fgla/shader_module.hpp>
#include <magic_enum/magic_enum.hpp>
#include <spdlog/spdlog.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

static constexpr int WIDTH = 1920;
static constexpr int HEIGHT = 1080;

int main(int argc, char **argv) {
  auto instance = "Failed to create instance" * fgla::Instance::create({
                                                    .app_version = {0, 0, 1},
                                                    .app_name = "Headless test",
                                                });

  auto adapters = fgla::util::FilterableList(instance.enumerate_adapters());

  auto adapter =
      "Failed to create adapter" *
      instance.select_adapter(instance.get_adapter_scorer({}), adapters.data());
  auto device = "Failed to create device" *
                adapter.create_device(
                    {{std::filesystem::path(__FILE__).parent_path()}},
                    {fgla::Queue::Request{fgla::Queue::Type::Graphics, 1}});

  fgla::Queue &graphics = *device.get_queue(fgla::Queue::Type::Graphics, 0);

  fgla::Format image_format = fgla::Format::R8G8B8A8_SRGB;
  fgla::Image image =
      "Failed to create image!" *
      device.create_image(
          {.memory_properties =
               {fgla::Memory::CpuAccess::NONE}, // remove memory_properties
           .usage = fgla::Image::Usage::COLOR_ATTACHMENT |
                    fgla::Image::Usage::TRANSFER_SRC,
           .dimension = fgla::Image::Dimension::D2,
           .format = image_format,
           .extent = fgla::Extent3d{WIDTH, HEIGHT, 1}});

  fgla::Buffer staging_buffer =
      "Failed to create buffer!" *
      device.create_buffer({
          .memory_properties = {fgla::Memory::CpuAccess::READ},
          .size = 4 * WIDTH * HEIGHT,
          .usage = fgla::Buffer::Usage::TRANSFER_DST,
      });

  auto shader =
      "Failed to load shader module" * device.load_shader_module({"test"});

  fgla::RenderPipeline pipeline =
      "Failed to create pipeline" *
      device.create_render_pipeline(
          {.vertex = {.module = shader, .entry_point = "vs_main"},
           .primitive = {.topology = fgla::RenderPipeline::PrimitiveState::
                             Topology::TRIANGLE_LIST},
           .fragment = {{.module = shader, .entry_point = "fs_main"}},
           .color_attachments = {{.format = image_format}}});

  auto image_view =
      "Failed to create image view!" *
      image.create_view({.format = image_format,
                         .mode = fgla::ImageView::Mode::D2,
                         .subresource_range = {
                             .aspect_flags = fgla::Image::AspectBits::COLOR,
                             .base_mip_level = 0,
                             .num_mip_levels = 1,
                             .base_array_layer = 0,
                             .num_array_layers = 1,
                         }});

  fgla::CommandBuffer cb =
      "Failed to create command buffer!" * graphics.begin_recording();

  {
    fgla::RenderPass pass =
        "Failed to begin render pass!" *
        cb.begin_render_pass(
            {.color_attachments = {
                 {.view = image_view,
                  .load_op =
                      fgla::RenderPass::LoadOp::CLEAR({0.2f, 0.3f, 0.9f, 1.0f}),
                  .store_op = fgla::RenderPass::StoreOp::STORE}}});

    pass.draw({.pipeline = pipeline, .vertex_count = 3});
  }

  cb.copy(image, staging_buffer,
          {fgla::CommandBuffer::BufferImageCopy{
              .buffer_offset = 0,
              .image_subresource =
                  {
                      .aspect_flags =
                          fgla::Image::SubresourceRange::AspectBits::COLOR,
                  },
              .image_offset = {0, 0, 0},
              .image_extent = {WIDTH, HEIGHT, 1},
          }});

  image.get_completion() =
      "Failed to submit command buffer!" * graphics.submit(std::move(cb), {});

  image.get_completion().wait();

  uint8_t *image_data = new uint8_t[WIDTH * HEIGHT * 4];

  {
    fgla::Memory::AccessConst access =
        "Failed to access staging buffer memory!" *
        staging_buffer.get_memory().access();

    memcpy(image_data, access.read(), WIDTH * HEIGHT * 4);
  }

  stbi_write_png("rainbow_dorito.png", WIDTH, HEIGHT,
                 4, // channels: RGBA
                 image_data,
                 WIDTH * 4 // stride: bytes per row
  );

  delete[] image_data;
}
