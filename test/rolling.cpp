#include <result/result.hpp>
#include <iostream>
#include <cstdlib>
#include <spdlog/spdlog.h>
#include <fgla/fgla.hpp>

static const unsigned int WIDTH = 800;
static const unsigned int HEIGHT = 600;

int main(int argc, char **argv) {
	fgla::Instance inst = fgla::Instance();
	if (!inst.is_ok()) {
		spdlog::error("main: fgla::Instance creation failed!");
		return -1;
	}
	
	fgla::Device device;

	fgla::Window window = fgla::Window(inst, WIDTH, HEIGHT, "FGLA test application");
	if (!window.is_ok()) {
		spdlog::error("main: fgla::Window creation failed!");
		return -1;
	}

	auto devices = fgla::Device::enumerate(inst, window);

	if (devices.size() == 0) {
		spdlog::error("main: No devices available!");
		return -1;
	}

	spdlog::info("main: {} devices available.", devices.size());
	for (const auto& device : devices) {
		spdlog::info("main: \t{} (scored {}, {} vram)", device.name(), device.score(), fgla::format_memory_size(device.available_memory()));
	}

	auto best_device = std::max_element(devices.begin(), devices.end(),
		[](const auto& a, const auto& b) {
			return a.score() < b.score();
		});

	device = *best_device;
	
	spdlog::info("main: Selecting device {}.", device.name());

	devices = {}; // free up memory

	auto res = device.initialize();
	if (res.has_error()) {
		spdlog::error("main: fgla::Device failed to initialize!");
		return -1;
	}

	window.run();

	return 0;
}
