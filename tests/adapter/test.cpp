#include <iostream>
#include <iomanip>
#include <fgla/backend.hpp>
#include <fgla/instance.hpp>

void print_uuid(const fgla::backend::BackendUUID &uuid) {
	for (int i = 0; i < 16; ++i) {
		std::cout << std::hex << std::setw(2) << std::setfill('0')
			<< static_cast<int>(uuid.bytes[i]);
	if (i == 3 || i == 5 || i == 7 || i == 9)
		std::cout << '-';
	}
	std::cout << std::dec; // reset to decimal
}

void print_backend(const fgla::backend::Backend& backend) {
	std::cout << backend.name << " [UUID: ";
	print_uuid(backend.uuid);
	std::cout << "]";
}
int main(int argc, char **argv) {
	std::cout << " -- Test Adapter Acquisition -- \n" << std::endl;

	auto instance_res = fgla::Instance::create({ nullptr });

	if (!instance_res) {
		std::cout << "Failed to create instance!" << std::endl;
		if (instance_res.error().message) {
			std::cerr << "Message: " << instance_res.error().message.value() << std::endl;
		}
		return -1;
	}
	fgla::Instance instance = std::move(*instance_res);

	auto adapter_res = instance.get_adapter({});

	if (!adapter_res) {
		std::cout << "Failed to get adapter!" << std::endl;
		if (adapter_res.error().message) {
			std::cerr << "Message: " << adapter_res.error().message.value() << std::endl;
		}
		return -2;
	}

	std::cout << "Success!" << std::endl;

	return 0;
}
