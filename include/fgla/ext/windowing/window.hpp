#pragma once

#include <cstdint>
#include <fgla/ext/windowing/surface.hpp>
#include <fgla/ext/windowing/event.hpp>
#include <fgla/object_gen.hpp>
#include <fgla/types.hpp>

namespace fgla::ext::windowing {

#define FGLA_OBJ_NAME Window

/*
 *  Represents a window, which may be used to draw to and interface with input
 *  devices
 */
FGLA_OBJ_START
/*
 *  Represents the settings used to create a `Window`
 */
struct Descriptor {
  /*
   *  The width of the window, in pixels
   */
  uint32_t width = 800;
  /**
   *  The height of the window, in pixels
   */
  uint32_t height = 600;
  /**
   * Whether the window size may be changed
   * by the window manager
   */
  bool resizable = false;
  /*
   *  The name or title of the window
   */
  const char *name = "FGLA Window";
};

#define FGLA_OBJ_FUNCTIONS(FN)                                                 \
  FN(/**                                                                       \
      * Internally runs the window's event loop and returns a vector containing\
      * new events since the last poll, if any. This must be called once per   \
      * frame. There may be at most one instance of every event type (e.g.     \
      * KeyEvent, MouseEvent, and so on) in the returned vector. The first call\
      * to this function must return a list with at least a MouseEvent         \
      * containing the current mouse position (with a delta of 0).             \
      */                                                                       \
     , std::vector<Event>, poll_events, (), ())                                \
  FN(/**                                                                       \
      * Checks if the window has **not** been closed by the user               \
      * @returns `false` if the user has closed the window, else `true`        \
      */                                                                       \
     , bool, is_open, (), ())                                                  \
  FN(/**                                                                       \
      * Creates a `Surface`, which is used to render to the `Window`           \
      * @param instance the `Instance` to use when creating the `Surface`      \
      * @returns a `Surface` which may be used to render to this `Window`,     \
      * or an `Error` containing failure information                           \
      */                                                                       \
     , Result<Surface>, create_surface, (const fgla::Instance &instance),      \
     (instance))                                                               \
  FN(/**                                                                       \
      * @returns The size of this `Window`'s framebuffer, in pixels            \
      */                                                                       \
     , Extent2d, get_framebuffer_size, () const, ())

FGLA_OBJ_END

#undef FGLA_OBJ_NAME
#undef FGLA_OBJ_FUNCTIONS

} // namespace fgla::ext::windowing
