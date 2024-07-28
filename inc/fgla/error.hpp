#pragma once

#include <string>
#include <any>
#include <stdexcept>

namespace fgla {


struct Result {
    enum class Status {
        SUCCESS = 0,
        GL_ERROR,
        // add more when needed
    };

    Status status;
    std::string desc = "";
    std::any data = std::any();
};
}