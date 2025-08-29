#pragma once

#include <fgla/instance.hpp>
#include <fgla/internal.hpp>
#include <fgla/types.hpp>
#include <memory>
#include <optional>
#include <vector>

namespace fgla::ext::windowing {

/// Represents a surface, which is used to render to a window
class Surface {
public:
  enum class PresentMode { FIFO, MAILBOX, IMMEDIATE, AUTO_VSYNC, AUTO_NO_VSYNC };

  struct Configuration {
    TextureFormat format;
    PresentMode present_mode;
    fgla::Extent2d size;
  };

  struct Capabilities {
    std::vector<TextureFormat> formats;
    std::vector<PresentMode> present_modes;
  };

  inline std::optional<Error> configure(fgla::Device &device, const Configuration &config) {
    return this->impl->configure(device, config);
  }
  inline Capabilities get_capabilities(const Adapter &adapter) {
    return this->impl->get_capabilities(adapter);
  }

  /// The backend-defined implementation of the `Surface`'s functions
  struct Impl {
    virtual std::optional<Error> configure(fgla::Device &, const Configuration &) = 0;
    virtual Capabilities get_capabilities(const Adapter &) = 0;
    virtual ~Impl() = 0;
  };

  /// Creates a `Surface` from a raw implementation
  /// This should only be used internally
  static inline Surface from_raw(std::shared_ptr<Impl> impl) {
    Surface surface;
    surface.impl = std::move(impl);
    return surface;
  }

private:
  friend struct fgla::internal::ImplAccessor;
  std::shared_ptr<Impl> impl;
};

inline Surface::Impl::~Impl() = default;

struct QueueTypeExt {
  static constexpr Queue::Type Present =
      Queue::Type(16); // a better way to do this is likely a good idea
};
struct PresentQueueOptions {
  const Surface &surface;
  const Adapter &adapter;
};
} // namespace fgla::ext::windowing
