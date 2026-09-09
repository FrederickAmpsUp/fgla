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

  const uint32_t data[] = {
      0x01234567, 0x89ABCDEF, 0x01234567, 0x89ABCDEF,
      0x01234567, 0x89ABCDEF, 0x01234567, 0x89ABCDEF,
  };

  fgla::Buffer buffer =
      "Failed to create buffer!" *
      device.create_buffer(
          {.memory = {.size = sizeof(data),
                      .cpu_access = fgla::Memory::CpuAccess::READ_WRITE},
           .usage = fgla::Buffer::Usage::VERTEX});

  fgla::Memory &memory = buffer.get_memory();

  {
    fgla::Memory::AccessMut access =
        "Failed to access buffer memory!" * memory.access();

    std::byte *ptr = access.write();
    memcpy(ptr, data, sizeof(data));
  }

  const fgla::Memory &cmemory = memory;

  {
    fgla::Memory::AccessConst access =
        "Failed to access buffer memory!" * cmemory.access();

    if (memcmp(access.read(), data, sizeof(data))) {
      spdlog::warn("Readback data does not match!");
    }
  }

  return 0;
}
