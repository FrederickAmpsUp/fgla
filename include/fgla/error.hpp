#pragma once

#include <cstdint>
#include <fgla/util.hpp>
#include <iostream>
#include <optional>
#include <string>
#include <tl/expected.hpp>

namespace fgla {

enum class ErrorCode : uint64_t {
  CREATE_INSTANCE_FAILED,
  CREATE_DEVICE_FAILED,
  GET_ADAPTER_FAILED,
  NO_BACKENDS,
  UNIMPLEMENTED,
  UNKNOWN,
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

template <typename T>
T unwrap_or_display_and_exit(tl::expected<T, Error> res,
                             const char *message = "Fatal Error:", int exit_code = -1) {
  if (!res) {
    std::cerr << message << ": \"" << res.error().message.value_or("Unknown error") << "\""
              << std::endl;
    std::exit(exit_code);
  } else {
    return std::move(*res);
  }
}

template <typename T>
T unwrap_or_display_and_exit(std::optional<T> opt, const char *message = "Fatal Error",
                             int exit_code = -1) {
  if (!opt) {
    std::cerr << message << std::endl;
    std::exit(exit_code);
  } else {
    return std::move(*opt);
  }
}

template <typename T>
T &unwrap_or_display_and_exit(util::OptRef<T> opt, const char *message = "Fatal Error",
                              int exit_code = -1) {
  if (!opt) {
    std::cerr << message << std::endl;
    std::exit(exit_code);
  } else {
    return *opt;
  }
}
} // namespace fgla
