#pragma once

#include <fgla/adapter.hpp>
#include <fgla/error.hpp>
#include <fgla/ext/windowing/surface.hpp>
#include <fgla/ext/windowing/window.hpp>
#include <fgla/extension.hpp>

namespace fgla::ext::windowing {

/// @brief Extension interface for the builtin windowing extension
///
/// Use `Instance::get_extension<fgla::ext::windowing::WindowingExtension>()` to get a handle to it
class WindowingExtension {
public:
  /// The UUID for the `WindowingExtension`
  static constexpr extension::ExtensionUUID UUID =
      extension::ExtensionUUID::parse_ctr("d8b41889-e4df-4e93-a115-0a828cc832c2");

  /// Attempts to create a `Window`
  /// @param _ The `Window::Descriptor` with the window's properties
  /// @returns The created `Window`, or an `Error` with failure information
  virtual Result<Window> create_window(const Window::Descriptor &) = 0;

  /// Returns an `Adapter` filtering function
  ///
  /// @param _ The `Surface` to filter against
  /// @returns A filter that checks if an `Adapter` supports the specified `Surface`
  virtual std::function<bool(const Adapter &)> surface_support_filter(const Surface &) = 0;

  virtual ~WindowingExtension() = 0;

private:
};

inline WindowingExtension::~WindowingExtension() = default;

} // namespace fgla::ext::windowing
