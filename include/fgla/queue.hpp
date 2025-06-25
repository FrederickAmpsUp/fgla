#pragma once

#include <memory>
#include <cstdint>

namespace fgla {

class Queue {
public:
	enum class Type {
		Graphics, // TODO: transfer/compute queues
	};

	struct Request {
		Type type;
		uint32_t count;
	};

	struct Impl {
		virtual ~Impl() = 0;
	};
private:
	std::unique_ptr<Impl> impl;
};

inline Queue::Impl::~Impl() = default;
}

