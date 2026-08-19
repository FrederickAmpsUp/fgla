#pragma once

#include <fgla/object_gen.hpp>

namespace fgla {

#define FGLA_OBJ_NAME Completion
/**
 * Signals the completion of a submitted operation,
 * which can be waited on or used to synchronize subsequent operations
 */
FGLA_OBJ_START

#define FGLA_OBJ_FUNCTIONS(FN)                                                 \
  FN(/**                                                                       \
      * Clones this `Completion`                                               \
      * @returns A clone of this `Completion`                                  \
      */                                                                       \
     , Completion, clone, () const, ())

FGLA_OBJ_END

#undef FGLA_OBJ_NAME
#undef FGLA_OBJ_FUNCTIONS

} // namespace fgla
