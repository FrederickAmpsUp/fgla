#pragma once

#include <memory>

namespace fgla {

class Adapter {
public:
	struct Descriptor {
		// TODO
	};

	struct Impl {
		virtual ~Impl();
	};

	static inline Adapter from_raw(std::unique_ptr<Impl> impl) {
		Adapter adapter;
		adapter.impl = std::move(impl);
		return adapter;
	}
private:
	std::unique_ptr<Impl> impl;
};

inline Adapter::Impl::~Impl() = default;

}
