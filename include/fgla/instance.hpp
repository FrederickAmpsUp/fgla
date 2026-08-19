#pragma once

#include <fgla/adapter.hpp>
#include <fgla/error.hpp>
#include <fgla/extension.hpp>
#include <fgla/internal.hpp>
#include <fgla/object_gen.hpp>
#include <fgla/util.hpp>
#include <functional>
#include <optional>

namespace fgla {

namespace backend {
struct Backend;
using BackendUUID = util::UUID<16>;
} // namespace backend

#define FGLA_OBJ_NAME Instance

/**
 * Represents an instance, which is used for extension
 * management and to acquire an `Adapter`
 */
FGLA_OBJ_START

/**
 * Stores the settings used to create an `Instance`
 */
struct Descriptor {
  /**
   * The `BackendUUID` of a backend to use over any others available
   */
  std::optional<backend::BackendUUID> preferred_backend;
  /**
   * A list of `ExtensionUUID`s for required extensions
   * If a backend does not support a required extension, it will not be used
   */
  std::vector<extension::ExtensionUUID> required_extensions;
  /**
   * The version of the application using `fgla`
   */
  VersionTriple app_version = {1, 0, 0};
  /**
   * The name of the application using `fgla`
   */
  std::string app_name = "";
};

/**
 * Creates an `Instance`
 * @param descriptor The `Descriptor` with the `Instance` creation settings
 * @returns The created `Instance`, or an `Error` with failure information
 */
static Result<Instance> create(const Descriptor &descriptor);

/**
 * Acquires an `Adapter`
 *  @param descriptor The `Adapter::Descriptor` with the `Adapter`'s
 *  properties
 *  @returns The `Adapter`, or an `Error` with failure information
 */
inline Result<Adapter> get_adapter(const Adapter::Descriptor &descriptor) {
  return this->select_adapter(this->get_adapter_scorer(descriptor),
                              this->enumerate_adapters());
}

/**
 *  Selects the `Adapter` with the highest score as indicated by `scorer`
 *  @param scorer The score function to score with
 *  @param adapters The list of adapters to select from
 *  @returns The highest-scoring `Adapter` from `adapters`
 *  @note The returned `Adapter` is removed from (moved out of) `adapters`
 */
Result<Adapter>
select_adapter(const std::function<int(const Adapter &)> &scorer,
               std::vector<Adapter> &adapters);

/**
 *  Alternative to `select_adapter(..., std::vector<Adapter> &)` that takes
 *  `adapters` as an rvalue.
 *  @see `select_adapter(..., std::vector<Adapter> &)`
 */
inline Result<Adapter>
select_adapter(const std::function<int(const Adapter &)> &scorer,
               std::vector<Adapter> &&adapters) {
  return this->select_adapter(scorer, adapters);
}

/**
 *  Retrieves an extension from its type
 *  @tparam T The extension to retrieve
 *  @returns The extension, or `std::nullopt` if not supported
 */
template <typename T> std::optional<std::reference_wrapper<T>> get_extension() {
  extension::ExtensionUUID uuid = T::UUID;
  void *ext = this->get_extension_raw(uuid);
  if (ext) {
    return *static_cast<T *>(ext);
  } else {
    return {};
  }
}

#define FGLA_OBJ_FUNCTIONS(FN)                                                 \
  FN(/**                                                                       \
      * @returns a list of all available `Adapter`s                            \
      */                                                                       \
     , std::vector<Adapter>, enumerate_adapters, (), ())                       \
  FN(/**                                                                       \
      * @returns a function that scores an `Adapter`                           \
      * based on an `Adapter::Descriptor`                                      \
      */                                                                       \
     , std::function<int(const Adapter &)>, get_adapter_scorer,                \
     (const Adapter::Descriptor &descriptor), (descriptor))                    \
  FN(/**                                                                       \
      * @returns The `Backend` this `Instance` is using                        \
      */                                                                       \
     , const backend::Backend &, get_backend, (), ())                          \
  FN(private:, void *, get_extension_raw, (extension::ExtensionUUID uuid),     \
     (uuid))
FGLA_OBJ_END

#undef FGLA_OBJ_NAME
#undef FGLA_OBJ_FUNCTIONS

} // namespace fgla
