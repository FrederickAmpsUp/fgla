#pragma once

#include <fgla/device.hpp>
#include <fgla/error.hpp>
#include <fgla/object_gen.hpp>

namespace fgla {

#define FGLA_OBJ_NAME Adapter

/** Represents an adapter, which is used to set up and create a `Device` */
FGLA_OBJ_START

/** Stores the settings used to create an `Adapter` */
struct Descriptor {
  // TODO
};

/** Stores information about an `Adapter` */
struct Info {
  std::string device_name;
};

#define FGLA_OBJ_FUNCTIONS(FN)                                                 \
  FN(/**                                                                       \
      * Attempts to create a `Device`                                          \
      * @param descriptor The `Device::Descriptor` with the `Device`'s         \
      * properties                                                             \
      * @param queues A list of `Queue::Requests` to create `Queue`s for       \
      * @returns The created `Device`, or an `Error` with failure information  \
      */                                                                       \
     , Result<Device>, create_device,                                          \
     (const Device::Descriptor &desc,                                          \
      std::initializer_list<Queue::Request> queues),                           \
     (desc, queues))                                                           \
  FN(/**                                                                       \
      * Retrieves this `Adapter`'s `Info`                                      \
      */                                                                       \
     , Info, get_info, () const, ())

FGLA_OBJ_END

#undef FGLA_OBJ_NAME
#undef FGLA_OBJ_FUNCTIONS

} // namespace fgla
