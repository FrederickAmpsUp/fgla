#pragma once

#include <fgla/object_gen.hpp>
#include <string>

namespace fgla {

#define FGLA_OBJ_NAME ShaderModule

FGLA_OBJ_START

/**
 * Describes the settings used to load a `ShaderModule`
 */
struct Descriptor {
  /**
   * The name of the shader module to load
   *
   * @note The module is searched for as a `.slang` file in
   * the paths specified by `Device::Descriptor::shader_paths`
   */
  const std::string &module_name;
};

#define FGLA_OBJ_FUNCTIONS(FN)

FGLA_OBJ_END

#undef FGLA_OBJ_NAME
#undef FGLA_OBJ_FUNCTIONS

} // namespace fgla
