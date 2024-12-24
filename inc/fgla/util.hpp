#pragma once

#include <result/result.hpp>
#include <string>
#include <iomanip>
#include <sstream>

namespace fgla {

using result = cpp::result<void, std::string>;

std::string format_memory_size(size_t bytes);
}
