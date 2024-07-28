#pragma once

#include <fgla/error.hpp>

namespace fgla {

/**
 * @brief Initialize the FGLA context
 * Sets up all context related data like the window library and any neccesary graphics library setup
 */
[[nodiscard]] fgla::Result initialize_context();
}