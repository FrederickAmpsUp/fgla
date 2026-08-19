#pragma once

#include <memory>
#include <type_traits>

#define FGLA_FN_BODY(doc, ret, name, args, fwd)                                \
  doc inline ret name args { return this->impl->name fwd; }                    \
                                                                               \
public:

#define FGLA_FN_IMPL(doc, ret, name, args, fwd)                                \
  doc virtual ret name args = 0;                                               \
                                                                               \
public:

#define FGLA_OBJ_START                                                         \
  class FGLA_OBJ_NAME {                                                        \
  private:                                                                     \
    FGLA_OBJ_NAME() = default;                                                 \
                                                                               \
  public:                                                                      \
    FGLA_OBJ_NAME(const FGLA_OBJ_NAME &) = delete;                             \
    FGLA_OBJ_NAME &operator=(const FGLA_OBJ_NAME &) = delete;                  \
    FGLA_OBJ_NAME(FGLA_OBJ_NAME &&) = default;                                 \
    FGLA_OBJ_NAME &operator=(FGLA_OBJ_NAME &&) = default;

#define FGLA_OBJ_END                                                           \
  FGLA_OBJ_FUNCTIONS(FGLA_FN_BODY)                                             \
  struct Impl {                                                                \
    FGLA_OBJ_FUNCTIONS(FGLA_FN_IMPL)                                           \
    virtual ~Impl() = 0;                                                       \
    friend class FGLA_OBJ_NAME;                                                \
  };                                                                           \
  template <typename T, std::enable_if_t<std::is_base_of_v<Impl, T>, int> = 0> \
  T &to_impl() {                                                               \
    return static_cast<T &>(*this->impl);                                      \
  }                                                                            \
  template <typename T, std::enable_if_t<std::is_base_of_v<Impl, T>, int> = 0> \
  const T &to_impl() const {                                                   \
    return static_cast<const T &>(*impl);                                      \
  }                                                                            \
  FGLA_OBJ_NAME from_impl(std::unique_ptr<Impl> impl) {                        \
    FGLA_OBJ_NAME obj;                                                         \
    obj.impl = std::move(impl);                                                \
    return obj;                                                                \
  }                                                                            \
                                                                               \
private:                                                                       \
  std::unique_ptr<Impl> impl;                                                  \
  }                                                                            \
  ;                                                                            \
  inline FGLA_OBJ_NAME::Impl::~Impl() = default;
