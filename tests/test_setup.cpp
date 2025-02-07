#include <iostream>
#include <fgla/fgla.hpp>
#include <spdlog/spdlog.h>

// throws on error
template<typename T, typename E>
T operator~(cpp::result<T, E> res) {
	if (!res) throw res.error();
	return std::move(res.value());
}

void run() {
	fgla::Window window = ~fgla::Window::create({
		.title = "FGLA test program",
		.width = 1920,
		.height = 1080
	});

	fgla::Instance instance = ~fgla::Instance::create({
		.app_name = "FGLA test program",
		.app_version = { 0, 0, 1 },
		.window = &window
	});

}

int main(int argc, char **argv) {
	try {
		spdlog::info("Running");
		run();
		spdlog::info("Done");
	} catch (fgla::Error e) {
		spdlog::error("Error running: {}", e.message);
		return -1;
	}

	return 0;
}
