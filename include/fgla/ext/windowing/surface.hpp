#pragma once

#include <fgla/completion.hpp>
#include <fgla/image.hpp>
#include <fgla/instance.hpp>
#include <fgla/object_gen.hpp>
#include <fgla/types.hpp>
#include <optional>
#include <vector>

namespace fgla::ext::windowing {

#define FGLA_OBJ_NAME Surface

/**
 * Represents a surface, which is used to render to a window
 */
FGLA_OBJ_START

enum class PresentMode { FIFO, MAILBOX, IMMEDIATE, AUTO_VSYNC, AUTO_NO_VSYNC };

struct Configuration {
  Format format;
  PresentMode present_mode;
  fgla::Extent2d size;
};

struct Capabilities {
  std::vector<Format> formats;
  std::vector<PresentMode> present_modes;
};

#define FGLA_OBJ_FUNCTIONS(FN)                                                 \
  FN(/**                                                                       \
      * @todo TODO                                                             \
      */                                                                       \
     , std::optional<Error>, configure,                                        \
     (fgla::Device & device, const Configuration &config), (device, config))   \
  FN(/**                                                                       \
      * @todo TODO                                                             \
      */                                                                       \
     , Capabilities, get_capabilities, (const Adapter &adapter), (adapter))    \
  FN(/**                                                                       \
      * @todo TODO                                                             \
      */                                                                       \
     , fgla::Result<std::reference_wrapper<fgla::Image>>, get_current_image,   \
     (fgla::Queue & queue), (queue))                                           \
  FN(/**                                                                       \
      * @todo TODO                                                             \
      */                                                                       \
     , std::optional<fgla::Error>, present,                                    \
     (fgla::Queue & queue, fgla::Image && image,                               \
      std::initializer_list<fgla::Completion> wait_completions = {}),          \
     (queue, std::move(image), wait_completions))                              \
  FN(/**                                                                       \
      * @todo TODO                                                             \
      */                                                                       \
     , void, cleanup, (), ())

FGLA_OBJ_END

#undef FGLA_OBJ_NAME
#undef FGLA_OBJ_FUNCTIONS

struct QueueTypeExt {
  static constexpr Queue::Type Present =
      Queue::Type(16); // a better way to do this is likely a good idea
};
struct PresentQueueOptions {
  const Surface &surface;
  const Adapter &adapter;
};
} // namespace fgla::ext::windowing
