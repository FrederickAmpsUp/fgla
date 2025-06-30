#pragma once

#include <fgla/ext/windowing/window.hpp>
#include <fgla/extension.hpp>

namespace fgla::ext::windowing {

class WindowExtension {
public:
  static constexpr extension::ExtensionUUID UUID =
      extension::ExtensionUUID::parse_ctr("d8b41889-e4df-4e93-a115-0a828cc832c2");

  virtual Window create_window(const Window::Descriptor &) = 0;
  virtual ~WindowExtension() = 0;

private:
};

inline WindowExtension::~WindowExtension() = default;

} // namespace fgla::ext::windowing
