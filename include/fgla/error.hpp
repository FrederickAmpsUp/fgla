#pragma once

#include <cstdint>
#include <fgla/util.hpp>
#include <iostream>
#include <optional>
#include <string>
#include <tl/expected.hpp>

namespace fgla {

/// Represents a failure state from a function call
struct Error {
  Error(uint64_t code, std::string message="") {
    this->code = code;
    if (message.size() > 0) this->message = message;
  }

  /// A number representing the failure state
  uint64_t code;
  /// Optionally, a message describing the failure
  std::optional<std::string> message = {};
};

/// Returns the value stored in `res`
/// Displays an error message and exits the program if `res` contains an `Error`
/// @tparam T the type of object to unwrap (will be inferred from arguments)
/// @param res The result to unwrap
/// @param message The error message to display on failure
/// @param exit_code The exit code to use on faulure
/// @returns The unwrapped value
template <typename T>
T unwrap_with_message(tl::expected<T, Error> res,
                      const char *message = "Fatal Error:", int exit_code = -1) {
  if (!res) {
    std::cerr << message << ": \"" << res.error().message.value_or("Unknown error") << "\""
              << std::endl;
    std::exit(exit_code);
  } else {
    return std::move(*res);
  }
}

/// Returns the value stored in `opt`
/// Displays an error message and exits the program if `opt` contains no value
/// @tparam T the type of object to unwrap (will be inferred from arguments)
/// @param opt The `std::optional` to unwrap
/// @param message The error message to display on failure
/// @param exit_code The exit code to use on faulure
/// @returns The unwrapped value
template <typename T>
T unwrap_with_message(std::optional<T> opt, const char *message = "Fatal Error",
                      int exit_code = -1) {
  if (!opt) {
    std::cerr << message << std::endl;
    std::exit(exit_code);
  } else {
    return std::move(*opt);
  }
}

/// Returns the value stored in `opt`
/// Displays an error message and exits the program if `opt` contains no value
/// @tparam T the type of object to unwrap (will be inferred from arguments)
/// @param opt The `util::OptRef` to unwrap
/// @param message The error message to display on failure
/// @param exit_code The exit code to use on faulure
/// @returns The unwrapped value
template <typename T>
T &unwrap_with_message(util::OptRef<T> opt, const char *message = "Fatal Error",
                       int exit_code = -1) {
  if (!opt) {
    std::cerr << message << std::endl;
    std::exit(exit_code);
  } else {
    return *opt;
  }
}
} // namespace fgla
