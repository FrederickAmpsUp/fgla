#include <fgla/backend.hpp>
#include <iomanip>
#include <iostream>

void print_uuid(const fgla::backend::BackendUUID &uuid) {
  for (int i = 0; i < 16; ++i) {
    std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(uuid.bytes[i]);
    if (i == 3 || i == 5 || i == 7 || i == 9) std::cout << '-';
  }
  std::cout << std::dec; // reset to decimal
}

void print_backend_registry() {
  const auto &registry = fgla::backend::get_registry();
  std::cout << "Registered backends:\n";
  for (const auto &[uuid, info] : registry) {
    std::cout << " - " << info.name << " [UUID: ";
    print_uuid(uuid);
    std::cout << "]\n";
  }
}

int main(int argc, char **argv) {
  std::cout << " -- Test Backend Registry -- \n" << std::endl;

  print_backend_registry();

  return 0;
}
