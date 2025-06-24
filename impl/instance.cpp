#include <fgla/instance.hpp>
#include <fmt/core.h>

namespace fgla {

tl::expected<Instance, Error> Instance::create(const Instance::Descriptor& create_opts) {
	const backend::Backend *backend = create_opts.preferred_backend;

	const auto& registry = backend::get_registry();

	auto it = registry.begin();

	do {
		if (backend == nullptr && it != registry.end()) {
			backend = &it->second;
			it++;
		}

		if (backend != nullptr) {
			const auto& backend_ref = *backend;
			
			if (backend::is_available(backend_ref.uuid)) {
				Instance *raw_instance = backend->create_instance();
				
				if (!raw_instance)
					return tl::make_unexpected(Error(ErrorCode::CREATE_INSTANCE_FAILED, fmt::format("Failed to create an fgla::Instance after selecting backend \"{}\".", backend_ref.name)));
				
				std::unique_ptr<Instance> instance(raw_instance);
				Instance moved = std::move(*instance);
				instance.reset();
				return tl::expected<Instance, Error>(std::move(moved));
			}

			backend = nullptr;
		}
	} while (it != registry.end());

	return tl::make_unexpected(Error(ErrorCode::NO_BACKENDS, "Failed to create an fgla::Instance as there are no available backends."));
}

}
