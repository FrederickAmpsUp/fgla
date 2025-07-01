#include <fgla/backend.hpp>
#include <fgla/instance.hpp>
#include <fmt/format.h>

// TODO: proper error codes

namespace fgla {

tl::expected<Instance, Error> Instance::create(const Instance::Descriptor &descriptor) {
  const backend::Backend *backend = nullptr;

  const auto &registry = backend::get_registry();

  auto pref_it = registry.find(descriptor.preferred_backend.value_or(backend::BackendUUID()));

  if (pref_it != registry.end()) {
    backend = &pref_it->second;
  }

  auto it = registry.begin();

  do {
    if (backend == nullptr && it != registry.end()) {
      backend = &it->second;
      it++;
    }

    if (backend != nullptr) {
      const auto &backend_ref = *backend;

      if (backend::is_available(backend_ref.uuid)) {
        Instance *raw_instance = backend->create_instance(&descriptor);

        if (!raw_instance) // TODO: change this to try other backends
          return tl::make_unexpected(Error(0,
              fmt::format("Failed to create an fgla::Instance after selecting backend \"{}\".",
                          backend_ref.name)));

        std::unique_ptr<Instance> instance(raw_instance);
        Instance moved = std::move(*instance);
        instance.reset();
        return tl::expected<Instance, Error>(std::move(moved));
      }

      backend = nullptr;
    }
  } while (it != registry.end());

  return tl::make_unexpected(
      Error(0,
            "Failed to create an fgla::Instance as there are no available backends."));
}

} // namespace fgla
