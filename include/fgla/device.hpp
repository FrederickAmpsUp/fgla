#pragma once

#include <fgla/error.hpp>
#include <fgla/object_gen.hpp>
#include <fgla/queue.hpp>
#include <fgla/render_pipeline.hpp>
#include <fgla/shader_module.hpp>
#include <fgla/util.hpp>
#include <filesystem>
#include <optional>
#include <vector>

namespace fgla {

#define FGLA_OBJ_NAME Device
/**
 * Represents a device, which is used to create rendering objects
 */
FGLA_OBJ_START
/**
 * Represents the settings used to create a `Device`
 */
struct Descriptor {
public:
  /**
   * A number of directories in which to search for shader source modules
   */
  const std::vector<std::filesystem::path> &shader_paths;
};
#define FGLA_OBJ_FUNCTIONS(FN)                                                 \
  FN(/**                                                                       \
      * Retrieves a `Queue` of the given type and index                        \
      *                                                                        \
      * @param type The type of queue to retrieve                              \
      * @param index The index of the queue in the specified type              \
      * @returns The specified queue, or `std::nullopt` if not found           \
      */                                                                       \
     , std::optional<std::reference_wrapper<Queue>>, get_queue,                \
     (Queue::Type type, uint32_t index), (type, index))                        \
  FN(/**                                                                       \
      * Loads a shader module from a file                                      \
      *                                                                        \
      * @param desc The `ShaderModule::Descriptor` describing which module to  \
      * load                                                                   \
      * @returns The loaded `ShaderModule`, or an `Error` with failure         \
      * information                                                            \
      */                                                                       \
     , Result<ShaderModule>, load_shader_module,                               \
     (const ShaderModule::Descriptor &desc), (desc))                           \
  FN(/**                                                                       \
      * Creates a `RenderPipeline`                                             \
      *                                                                        \
      * @param desc The `RenderPipeline::Descriptor` containing settings for   \
      * the render pipeline to be created                                      \
      * @returns The created `RenderPipeline`, or an `Error` with failure      \
      * information                                                            \
      */                                                                       \
     , Result<RenderPipeline>, create_render_pipeline,                         \
     (const RenderPipeline::Descriptor &desc), (desc))

FGLA_OBJ_END

#undef FGLA_OBJ_NAME
#undef FGLA_OBJ_FUNCTIONS

} // namespace fgla
