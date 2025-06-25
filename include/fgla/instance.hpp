#pragma once

#include <memory>
#include <fgla/adapter.hpp>
#include <fgla/error.hpp>
#include <tl/expected.hpp>
#include <optional>

namespace fgla {

namespace backend {
struct Backend;
}

class Instance {
public:
	struct Descriptor {
		const backend::Backend *preferred_backend; // nullable, maybe should change to a UUID
	};

	static tl::expected<Instance, Error> create(const Descriptor& descriptor);

	inline tl::expected<Adapter, Error> get_adapter(const Adapter::Descriptor& descriptor) { return this->impl->get_adapter(descriptor); }
	inline const backend::Backend& get_backend() { return this->impl->get_backend(); }

	struct Impl {
		virtual tl::expected<Adapter, Error> get_adapter(const Adapter::Descriptor&) = 0;
		virtual const backend::Backend& get_backend() = 0;

		virtual ~Impl() = 0;
	};

	static inline Instance from_raw(std::unique_ptr<Instance::Impl> impl) {
		Instance inst;
		inst.impl = std::move(impl);
		return inst;
	}

private:
	std::unique_ptr<Impl> impl;
};

inline Instance::Impl::~Impl() = default;

}
