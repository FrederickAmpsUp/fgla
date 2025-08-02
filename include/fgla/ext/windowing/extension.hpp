#pragma once

#include <fgla/error.hpp>
#include <fgla/ext/windowing/window.hpp>
#include <fgla/extension.hpp>
#include <tl/expected.hpp>

namespace fgla::ext::windowing {

/// @brief Extension interface for the builtin windowing extension
///
/// Use `Instance::get_extension<fgla::ext::windowing::WindowingExtension>()` to get a handle to it
class WindowExtension {
public:
  /// The UUID for the `WindowingExtension`
  static constexpr extension::ExtensionUUID UUID =
      extension::ExtensionUUID::parse_ctr("d8b41889-e4df-4e93-a115-0a828cc832c2");

  /// Attempts to create a `Window`
  /// @param _ The `Window::Descriptor` with the window's properties
  /// @returns The created `Window`, or an `Error` with failure information
  virtual tl::expected<Window, Error> create_window(const Window::Descriptor &) = 0;

  virtual ~WindowExtension() = 0;

private:
};

inline WindowExtension::~WindowExtension() = default;

} // namespace fgla::ext::windowing
