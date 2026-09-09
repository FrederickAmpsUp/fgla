#pragma once

#include <cstddef>
#include <cstdint>
#include <fgla/error.hpp>
#include <fgla/object_gen.hpp>
#include <utility>

namespace fgla {

#define FGLA_OBJ_NAME Memory

/**
 * Represents a contiguous chunk of memory associated with a
 * `Buffer`, `Texture`, or similar object.
 */
FGLA_OBJ_START

enum class CpuAccess : uint8_t {
  NONE = 0,
  READ = 1 << 0,
  WRITE = 1 << 1,
  READ_WRITE = READ | WRITE
};

struct Descriptor {
  size_t size;
  CpuAccess cpu_access = CpuAccess::NONE;
};

#undef FGLA_OBJ_NAME
#define FGLA_OBJ_NAME AccessMut

class AccessConst;

/**
 * Represents temporary read/write CPU access to a contiguous
 * chunk of `Memory`.
 */
FGLA_OBJ_START

#define FGLA_OBJ_FUNCTIONS(FN)                                                 \
  FN(/**                                                                       \
      * @returns A read-only pointer to the                                    \
      * start of the accessed memory                                           \
      *                                                                        \
      * @note The returned pointer is valid for the lifetime of this access    \
      */                                                                       \
     , const std::byte *, read, () const, ())                                  \
  FN(/**                                                                       \
      * @returns A read-write pointer to the                                   \
      * start of the accessed memory                                           \
      *                                                                        \
      * @note The returned pointer is valid for the lifetime of this access    \
      */                                                                       \
     , std::byte *, write, (), ())

operator AccessConst() && {
  AccessConst access;
  access.impl = std::move(this->impl);
  return access;
}

FGLA_OBJ_END

/**
 * Represents temporary read-only CPU access to a contiguous
 * chunk of `Memory`.
 */
class AccessConst : private AccessMut {
public:
  using AccessMut::from_impl;
  using AccessMut::read;
  using AccessMut::to_impl;

  friend class AccessMut;
};

#undef FGLA_OBJ_NAME
#undef FGLA_OBJ_FUNCTIONS

#define FGLA_OBJ_NAME Memory
#define FGLA_OBJ_FUNCTIONS(FN)                                                 \
  FN(/**                                                                       \
      * Acquires CPU access to the `Memory`                                    \
      *                                                                        \
      * @returns An `AccessMut` providing read/write pointers to this          \
      * `Memory`, or an `Error` containing failure information                 \
      */                                                                       \
     , Result<AccessMut>, access, (), ())                                      \
  FN(/**                                                                       \
      * Acquires CPU access to the `Memory`                                    \
      *                                                                        \
      * @returns An `AccessConst` providing read-only pointers to this         \
      * `Memory`, or an `Error` containing failure information                 \
      */                                                                       \
     , Result<AccessConst>, access, () const, ())

FGLA_OBJ_END

#undef FGLA_OBJ_NAME
#undef FGLA_OBJ_FUNCTIONS

constexpr Memory::CpuAccess operator|(Memory::CpuAccess a,
                                      Memory::CpuAccess b) {
  return (Memory::CpuAccess)((uint8_t)a | (uint8_t)b);
}
constexpr Memory::CpuAccess operator&(Memory::CpuAccess a,
                                      Memory::CpuAccess b) {
  return (Memory::CpuAccess)((uint8_t)a & (uint8_t)b);
}

} // namespace fgla
