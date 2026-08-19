#include <fgla/backend.hpp>
#include <fgla/instance.hpp>
#include <fmt/format.h>
#include <functional>

// TODO: proper error codes

namespace fgla {

Result<Instance> Instance::create(const Instance::Descriptor &descriptor) {
  const backend::Backend *backend = nullptr;

  const auto &registry = backend::get_registry();

  auto pref_it = registry.find(
      descriptor.preferred_backend.value_or(backend::BackendUUID()));

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
          return Error(0, fmt::format("Failed to create an fgla::Instance "
                                      "after selecting backend \"{}\".",
                                      backend_ref.name));

        std::unique_ptr<Instance> instance(raw_instance);
        Instance moved = std::move(*instance);
        instance.reset();
        return std::move(moved);
      }

      backend = nullptr;
    }
  } while (it != registry.end());

  return Error(
      0,
      "Failed to create an fgla::Instance as there are no available backends.");
}

Result<Adapter>
Instance::select_adapter(const std::function<int(const Adapter &)> &scorer,
                         std::vector<Adapter> &adapters) {

  size_t best_index;
  int best_score = -1;

  size_t i = 0;
  for (const Adapter &adapter : adapters) {
    int score = scorer(adapter);
    if (score > best_score) {
      best_score = score;
      best_index = i;
    }
    ++i;
  }

  if (best_score < 0) {
    return Error(0, "No suitable adapters found");
  }

  Adapter best_adapter = std::move(adapters[best_index]);
  adapters.erase(adapters.begin() + best_index);
  return best_adapter;
}

} // namespace fgla
