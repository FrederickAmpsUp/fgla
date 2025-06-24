#pragma once

#include <optional>
#include <cstdint>
#include <string>

namespace fgla {

enum class ErrorCode : uint64_t {
	CREATE_INSTANCE_FAILED,
	NO_BACKENDS,
	UNIMPLEMENTED,
	END
};

struct Error {
	inline Error(ErrorCode c) : Error(static_cast<uint64_t>(c)) {}
	inline Error(ErrorCode c, std::string m) : Error(static_cast<uint64_t>(c), m) {}
	inline Error(uint64_t c) : code(c) {}
	inline Error(uint64_t c, std::string m) : code(c), message(m) {}

	uint64_t code;
	std::optional<std::string> message;

	std::string to_string();
};
}
