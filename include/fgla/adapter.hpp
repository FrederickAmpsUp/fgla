#pragma once

#include <memory>

namespace fgla {

class Adapter {
public:
	struct Descriptor {
		// TODO
	};

	struct Info {
		std::string device_name;
	};

	inline Info get_info() const { return this->impl->get_info(); };

	struct Impl {
		virtual Info get_info() const = 0;

		virtual ~Impl() = 0;
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
