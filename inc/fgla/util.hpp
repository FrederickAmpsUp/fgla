#pragma once

#include <cstdint>
#include <result/result.hpp>
#include <any>

namespace fgla {

struct Version {
	uint32_t major = 0;
	uint32_t minor = 0;
	uint32_t patch = 0;
};

struct Error {
	std::string message = "";
	std::any data;
};

}
