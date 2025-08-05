#pragma once

#include <cstdint>
#include <fgla/util.hpp>
#include <iostream>
#include <optional>
#include <string>

namespace fgla {

/// Represents a failure state from a function call
struct Error {
  Error(uint64_t code, std::string message = "") {
    this->code = code;
    if (message.size() > 0) this->message = message;
  }

  /// A number representing the failure state
  uint64_t code;
  /// Optionally, a message describing the failure
  std::optional<std::string> message = {};
};

/// @brief Represents a potential success or failure state from a function call
/// @tparam T The result type representing success
/// @tparam E The result type representing failure (defaults to `Error`)
template <typename T, typename E = Error> class Result {
public:
  /// @brief Creates a new success result
  /// @param t The success object
  inline Result(T &&t) : t(std::move(t)), ok(true) {}
  inline Result() : e(), ok(false) {}
  /// @brief Creates a new failure result
  /// @param e The failure object
  inline Result(E &&e) : e(std::move(e)), ok(false) {}

  /// Checks if the result is in a success state
  inline operator bool() const { return this->ok; }
  /// Checks if the result is in a success state
  inline bool is_ok() const { return this->ok; }
  /// Checks if the result is in a success state
  inline bool has_value() const { return this->ok; }
  /// Checks if the result is in an error state
  inline bool has_error() const { return !this->ok; }
  
  /// @brief Retrieves the success object
  /// @note asserts if `this` is in an error state
  inline T &operator*() & {
    assert(this->ok && "Result::operator* called on error");
    return t;
  }
  /// @brief Retrieves the success object
  /// @note asserts if `this` is in an error state
  inline const T &operator*() const & {
    assert(this->ok && "Result::operator* called on error");
    return t;
  }
  /// @brief Retrieves the success object
  /// @note asserts if `this` is in an error state
  inline T &&operator*() && {
    assert(this->ok && "Result::operator* called on error");
    return std::move(t);
  }
  /// @brief Retrieves the success object
  /// @note asserts if `this` is in an error state
  inline const T &&operator*() const && {
    assert(this->ok && "Result::operator* called on error");
    return std::move(t);
  }
  /// @brief Retrieves the success object
  /// @note asserts if `this` is in an error state
  inline T *operator->() & {
    assert(this->ok && "Result::operator-> called on error");
    return &t;
  }
  /// @brief Retrieves the success object
  /// @note asserts if `this` is in an error state
  inline const T *operator->() const & {
    assert(this->ok && "Result::operator-> called on error");
    return &t;
  }
  /// @brief Retrieves the success object
  /// @note asserts if `this` is in an error state
  inline T &value() & {
    assert(this->ok && "Result::value called on error");
    return t;
  }
  /// @brief Retrieves the success object
  /// @note asserts if `this` is in an error state
  inline const T &value() const & {
    assert(this->ok && "Result::value called on error");
    return t;
  }
  /// @brief Retrieves the success object
  /// @note asserts if `this` is in an error state
  inline T &&value() && {
    assert(this->ok && "Result::value called on error");
    return std::move(t);
  }
  /// @brief Retrieves the success object
  /// @note asserts if `this` is in an error state
  inline const T &&value() const && {
    assert(this->ok && "Result::value called on error");
    return std::move(t);
  }
  /// @brief Retrieves the error object
  /// @note asserts if `this` is in a success state
  inline E &error() & {
    assert(!this->ok && "Result::error called on success");
    return e;
  }
  /// @brief Retrieves the error object
  /// @note asserts if `this` is in a success state
  inline const E &error() const & {
    assert(!this->ok && "Result::error called on success");
    return e;
  }
  /// @brief Retrieves the error object
  /// @note asserts if `this` is in a success state
  inline E &&error() && {
    assert(!this->ok && "Result::error called on success");
    return std::move(e);
  }
  /// @brief Retrieves the error object
  /// @note asserts if `this` is in a success state
  inline const E &&error() const && {
    assert(!this->ok && "Result::error called on success");
    return std::move(e);
  }
  inline ~Result() {
    if (this->ok) {
      this->t.~T();
    } else {
      this->e.~E();
    }
  }

private:
  bool ok;
  union {
    T t;
    E e;
  };
};

/// @brief Retrieves the success object from a `Result`, printing a custom message on failure
///
/// @param msg The message to print if `res` is in a failure state
/// @param res The result to unwrap
///
/// @note asserts if `res` is in an error state
///
/// @code
/// fgla::Instance instance = "Failed to create instance" *fgla::Instance::create({});
/// @endcode
template <typename T, typename E> T &operator*(const char *msg, Result<T, E> &res) {
  assert(res.is_ok() && msg);
  return *res;
}
/// @brief Retrieves the success object from a `Result`, printing a custom message on failure
///
/// @param msg The message to print if `res` is in a failure state
/// @param res The result to unwrap
///
/// @note asserts if `res` is in an error state
///
/// @code
/// fgla::Instance instance = "Failed to create instance" *fgla::Instance::create({});
/// @endcode
template <typename T, typename E> const T &operator*(const char *msg, const Result<T, E> &res) {
  assert(res.is_ok() && msg);
  return *res;
}
/// @brief Retrieves the success object from a `Result`, printing a custom message on failure
///
/// @param msg The message to print if `res` is in a failure state
/// @param res The result to unwrap
///
/// @note asserts if `res` is in an error state
///
/// @code
/// fgla::Instance instance = "Failed to create instance" *fgla::Instance::create({});
/// @endcode
template <typename T, typename E> T &&operator*(const char *msg, Result<T, E> &&res) {
  assert(res.is_ok() && msg);
  return std::move(*res);
}
/// @brief Retrieves the success object from a `Result`, printing a custom message on failure
///
/// @param msg The message to print if `res` is in a failure state
/// @param res The result to unwrap
///
/// @note asserts if `res` is in an error state
///
/// @code
/// fgla::Instance instance = "Failed to create instance" *fgla::Instance::create({});
/// @endcode
template <typename T, typename E> const T &&operator*(const char *msg, const Result<T, E> &&res) {
  assert(res.is_ok() && msg);
  return std::move(*res);
}

/// Returns the value stored in `res`
/// Displays an error message and exits the program if `res` contains an error
/// @tparam T the success type of the `Result` to unwrap
/// @tparam E the failure type of the `Result` to unwrap
/// @param res The `Result` to unwrap
/// @param message The error message to display on failure
/// @param exit_code The exit code to use on failure
/// @returns The unwrapped value
template <typename T, typename E>
T unwrap(Result<T, E> res, const char *message = "Fatal Error", int exit_code = -1) {
  if (!res) {
    std::cerr << message << std::endl;
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
T unwrap(std::optional<T> opt, const char *message = "Fatal Error", int exit_code = -1) {
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
T &unwrap(util::OptRef<T> opt, const char *message = "Fatal Error", int exit_code = -1) {
  if (!opt) {
    std::cerr << message << std::endl;
    std::exit(exit_code);
  } else {
    return *opt;
  }
}
} // namespace fgla
