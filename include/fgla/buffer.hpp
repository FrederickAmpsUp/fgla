#pragma once

#include <fgla/memory.hpp>
#include <fgla/object_gen.hpp>

namespace fgla {

#define FGLA_OBJ_NAME Buffer

FGLA_OBJ_START

struct Descriptor {
  Memory::Descriptor memory;
  // TODO: buffer usage, etc
};

#define FGLA_OBJ_FUNCTIONS(FN)                                                 \
  FN(/**                                                                       \
      * @returns A handle to this `Buffer`'s internal `Memory`                 \
      */                                                                       \
     , Memory &, get_memory, (), ())                                           \
  FN(/**                                                                       \
      * @returns An immutable handle to this `Buffer`'s internal `Memory`      \
      */                                                                       \
     , const Memory &, get_memory, () const, ())

FGLA_OBJ_END

#undef FGLA_OBJ_NAME
#undef FGLA_OBJ_FUNCTIONS

} // namespace fgla
