#include <cstring>
#include <fgla/backend.hpp>
#include <fgla/buffer.hpp>
#include <fgla/instance.hpp>
#include <iomanip>
#include <iostream>
#include <spdlog/spdlog.h>

void print_uuid(const fgla::backend::BackendUUID &uuid) {
  for (int i = 0; i < 16; ++i) {
    std::cout << std::hex << std::setw(2) << std::setfill('0')
              << static_cast<int>(uuid.bytes[i]);
    if (i == 3 || i == 5 || i == 7 || i == 9) std::cout << '-';
  }
  std::cout << std::dec; // reset to decimal
}

void print_backend(const fgla::backend::Backend &backend) {
  std::cout << backend.name << " [UUID: ";
  print_uuid(backend.uuid);
  std::cout << "]";
}
int main(int argc, char **argv) {
  std::cout << " -- Test Device/Queue Acquisition -- \n" << std::endl;

  auto instance_res = fgla::Instance::create({{}});

  if (!instance_res) {
    std::cerr << "Failed to create instance!" << std::endl;
    if (instance_res.error().message) {
      std::cerr << "Message: " << instance_res.error().message.value()
                << std::endl;
    }
    return -1;
  }
  fgla::Instance instance = std::move(*instance_res);

  auto adapter_res = instance.get_adapter({});

  if (!adapter_res) {
    std::cerr << "Failed to get adapter!" << std::endl;
    if (adapter_res.error().message) {
      std::cerr << "Message: " << adapter_res.error().message.value()
                << std::endl;
    }
    return -2;
  }

  fgla::Adapter adapter = std::move(*adapter_res);

  auto adapter_info = adapter.get_info();

  std::cout << "Acquired adapter \"" << adapter_info.device_name << "\"."
            << std::endl;

  auto device_res = adapter.create_device(
      {{}}, {fgla::Queue::Request{fgla::Queue::Type::Transfer, 1}});

  if (!device_res) {
    std::cerr << "Failed to create device!" << std::endl;
    if (adapter_res.error().message) {
      std::cerr << "Message: " << adapter_res.error().message.value()
                << std::endl;
    }
  }

  fgla::Device device = std::move(*device_res);

  fgla::Queue &transfer = *device.get_queue(fgla::Queue::Type::Transfer, 0);

  const char data[] = "This data was transferred through the GPU!\n";

  fgla::Buffer buffer1 =
      "Failed to create buffer!" *
      device.create_buffer(
          {.memory = {.size = sizeof(data),
                      .cpu_access = fgla::Memory::CpuAccess::WRITE},
           .usage = fgla::Buffer::Usage::TRANSFER_SRC});

  fgla::Buffer buffer2 =
      "Failed to create buffer!" *
      device.create_buffer(
          {.memory = {.size = sizeof(data),
                      .cpu_access = fgla::Memory::CpuAccess::READ},
           .usage = fgla::Buffer::Usage::TRANSFER_DST});

  {
    fgla::Memory::AccessMut access =
        "Failed to access buffer memory!" * buffer1.get_memory().access();

    std::byte *ptr = access.write();
    memcpy(ptr, data, sizeof(data));
  }

  auto command_buf = "Failed to begin recording" * transfer.begin_recording();

  command_buf.copy_buffer(buffer1, buffer2, {{0, 0, sizeof(data)}});

  auto copy_completion =
      "Failed to submit" * transfer.submit(std::move(command_buf));

  copy_completion.wait();

  {
    fgla::Memory::AccessConst access =
        "Failed to access buffer memory!" * buffer2.get_memory().access();

    if (memcmp(access.read(), data, sizeof(data))) {
      spdlog::warn("Readback data does not match!");
    }

    printf("%s", (const char *)access.read());
  }

  return 0;
}
