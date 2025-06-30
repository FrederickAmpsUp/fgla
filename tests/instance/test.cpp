#include <fgla/backend.hpp>
#include <fgla/instance.hpp>
#include <iomanip>
#include <iostream>

void print_uuid(const fgla::backend::BackendUUID &uuid) {
  for (int i = 0; i < 16; ++i) {
    std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(uuid.bytes[i]);
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
  std::cout << " -- Test Instance Creation -- \n" << std::endl;

  auto instance = fgla::Instance::create({{}});

  if (instance.has_value()) {
    std::cout << "Success, with backend ";
    print_backend(instance->get_backend());
    std::cout << std::endl;
  } else {
    std::cerr << "Failure :(" << std::endl;
    std::cerr << instance.error().message.value_or("") << std::endl;
  }

  return 0;
}
