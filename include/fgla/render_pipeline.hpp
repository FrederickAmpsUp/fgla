#pragma once

#include <fgla/internal.hpp>
#include <fgla/shader_module.hpp>
#include <fgla/types.hpp>
#include <memory>
#include <optional>
#include <vector>

namespace fgla {

class RenderPipeline {
public:
  struct ShaderStage {
    const ShaderModule &module;
    const char *entry_point;
  };

  struct PrimitiveState {
    enum class Topology {
      POINT_LIST,
      LINE_LIST,
      LINE_STRIP,
      TRIANGLE_LIST,
      TRIANGLE_STRIP
    };

    Topology topology = Topology::TRIANGLE_LIST;
  };

  /* TODO: raster state */

  struct ColorAttachment {
    Format format;
  };

  struct Descriptor {
    /* TODO: pipeline layout OR a better named version */
    ShaderStage    vertex;
    PrimitiveState primitive;
    std::optional<ShaderStage> fragment;
    std::vector<ColorAttachment> color_attachments;
  };

  struct Impl {
    virtual ~Impl() = 0;
  };

  /// Creates a `RenderPipeline` from a raw implementation
  /// This should only be used internally
  static inline RenderPipeline from_raw(std::unique_ptr<Impl> impl) {
    RenderPipeline render_pipeline;
    render_pipeline.impl = std::move(impl);
    return render_pipeline;
  }
private:
  friend struct fgla::internal::ImplAccessor;
  std::unique_ptr<Impl> impl;
};

inline RenderPipeline::Impl::~Impl() = default;

} // namespace fgla
