#pragma once

#include <memory>
#include <fgla/queue.hpp>

namespace fgla {

class Device {
public:
	struct Descriptor {

	};

	Queue& get_queue(Queue::Type type, uint32_t index);

	struct Impl {
		virtual ~Impl() = 0;
	};
private:
	std::unique_ptr<Impl> impl;
};

inline Device::Impl::~Impl() = default;
}
