#pragma once

#include <fgla/object_gen.hpp>
#include <fgla/shader_module.hpp>
#include <fgla/types.hpp>
#include <optional>
#include <vector>

namespace fgla {

#define FGLA_OBJ_NAME RenderPipeline

FGLA_OBJ_START

/**
 * Describes a shader stage used by a `RenderPipeline`
 */
struct ShaderStage {
  /**
   * The shader module containing the stage
   */
  const ShaderModule &module;

  /**
   * The name of the entry point to execute.
   */
  const char *entry_point;
};

/**
 * Describes how primitives are assembled by a `RenderPipeline`
 */
struct PrimitiveState {
  /**
   * Determines how vertices are grouped into primitives
   */
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

/**
 * Describes a color attachment used by a `RenderPipeline`
 */
struct ColorAttachment {
  /**
   * The format of the color attachment
   */
  Format format;
};

/**
 * Stores the settings used to create a `RenderPipeline`
 */
struct Descriptor {
  /* TODO: pipeline layout OR a better named version */
  /**
   * The vertex shader stage used by the pipeline
   */
  ShaderStage vertex;
  /**
   * The primitive assembly settings used by the pipeline
   */
  PrimitiveState primitive;
  /**
   * The fragment shader stage used by the pipeline,
   * or `std::nullopt` it none is used
   */
  std::optional<ShaderStage> fragment;
  /**
   * The color attachments written by the pipeline
   */
  std::vector<ColorAttachment> color_attachments;
};

#define FGLA_OBJ_FUNCTIONS(FN)

FGLA_OBJ_END

#undef FGLA_OBJ_NAME
#undef FGLA_OBJ_FUNCTIONS

} // namespace fgla
