#pragma once

#include <fgla/window.hpp>
#include <memory>

namespace fgla {

class Instance {
public:
    struct NativeInstanceInfo;

    Instance(std::shared_ptr<fgla::Window> win);
    ~Instance();

    /**
     * @brief Return the internal handle to instance information.
     * This is defined per-implementation and generally should not be used.
     */
    NativeInstanceInfo *info() { return this->m_inf; }
private:
    std::shared_ptr<fgla::Window> m_window;

    NativeInstanceInfo *m_inf;
};
}