#ifndef FGLA_OBJ_NAME
#error "Please define FGLA_OBJ_NAME before including this file"
#endif

#ifndef FGLA_OBJ_FUNCTIONS
#define FGLA_OBJ_FUNCTIONS
#endif

#include <memory>

class FGLA_OBJ_NAME {
public:
  FGLA_OBJ_NAME() = default;

  // Delete copy
  FGLA_OBJ_NAME(const FGLA_OBJ_NAME &) = delete;
  FGLA_OBJ_NAME &operator=(const FGLA_OBJ_NAME &) = delete;

  // Allow move
  FGLA_OBJ_NAME(FGLA_OBJ_NAME &&) = default;
  FGLA_OBJ_NAME &operator=(FGLA_OBJ_NAME &&) = default;

#define FN(ret, name, args, fwd)                                               \
  ret name args { return this->impl->name fwd; }

  FGLA_OBJ_FUNCTIONS
#undef FN

#define FN(ret, name, args, _) virtual ret name args = 0;

  struct Impl {
    FGLA_OBJ_FUNCTIONS

    virtual ~Impl() = 0;
  };
#undef FN

  FGLA_OBJ_NAME from_raw(std::unique_ptr<Impl> impl) {
    FGLA_OBJ_NAME obj;
    obj.impl = std::move(impl);
    return obj;
  }

private:
  std::unique_ptr<Impl> impl;
};

inline FGLA_OBJ_NAME::Impl::~Impl() = default;

#undef FGLA_OBJ_NAME
#undef FGLA_OBJ_FUNCTIONS
