#pragma once

#include <cstdint>
#include <fgla/command_buffer.hpp>
#include <fgla/completion.hpp>
#include <fgla/error.hpp>
#include <fgla/object_gen.hpp>

namespace fgla {

#define FGLA_OBJ_NAME Queue

/**
 * Represents a logical queue, which is used to submit GPU commands
 */
FGLA_OBJ_START

/**
 * The type of a `Queue`, indicating which functions it may perform
 */
enum class Type { Graphics, Transfer };

/**
 * Describes the creation of a number of `Queue`s, all with a shared `Type`
 */
struct Request {
  /**
   * The type of the `Queues` to create
   */
  Type type;
  /**
   * The number of `Queues` of the given type to create
   */
  uint32_t count;
  /**
   * Any arbitrary data to use during queue creation
   *
   * @note This may be left null in most cases
   */
  void *user_data = nullptr;
};

#define FGLA_OBJ_FUNCTIONS(FN)                                                 \
  FN(/**                                                                       \
      * Starts recording commands to a `CommandBuffer`                         \
      *                                                                        \
      * @returns The `CommandBuffer`, ready to accept commands                 \
      */                                                                       \
     , Result<CommandBuffer>, begin_recording, (), ())                         \
  FN(/**                                                                       \
      * Submits a `CommandBuffer` so that its commands may be executed         \
      *                                                                        \
      * @param cb The `CommandBuffer` to submit                                \
      * @param wait_completions A numer of `Completion`s to wait on            \
      * before command execution is started for this `CommandBuffer`           \
      * @returns A `Completion` which will trigger when command execution is   \
      * finished, or an `Error` containing failure information                 \
      */                                                                       \
     , Result<Completion>, submit,                                             \
     (CommandBuffer && cb,                                                     \
      std::initializer_list<Completion> wait_completions),                     \
     (std::move(cb), wait_completions))

FGLA_OBJ_END

#undef FGLA_OBJ_NAME
#undef FGLA_OBJ_FUNCTIONS

} // namespace fgla
