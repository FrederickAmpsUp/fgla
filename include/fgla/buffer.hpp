#pragma once

#include <fgla/memory.hpp>
#include <fgla/object_gen.hpp>

namespace fgla {

#define FGLA_OBJ_NAME Buffer

FGLA_OBJ_START

enum class Usage : uint32_t {
  NONE = 0,
  VERTEX = 1 << 0,
  INDEX = 1 << 1,
  UNIFORM = 1 << 2,
  STORAGE = 1 << 3,
  INDIRECT = 1 << 4,
  TRANSFER_SRC = 1 << 5,
  TRANSFER_DST = 1 << 6,
};

struct Descriptor {
  Memory::Descriptor memory;
  Usage usage;
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

constexpr Buffer::Usage operator|(Buffer::Usage a, Buffer::Usage b) noexcept {
  return (Buffer::Usage)((uint32_t)a | (uint32_t)b);
}
constexpr Buffer::Usage operator&(Buffer::Usage a, Buffer::Usage b) noexcept {
  return (Buffer::Usage)((uint32_t)a & (uint32_t)b);
}
} // namespace fgla
