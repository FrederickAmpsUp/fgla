#pragma once

#include <cstdint>

namespace fgla::util {
constexpr uint8_t parse_hex(char c) {
    return
        (c >= '0' && c <= '9') ? c - '0' :
        (c >= 'a' && c <= 'f') ? c - 'a' + 10 :
        (c >= 'A' && c <= 'F') ? c - 'A' + 10 :
        throw "Invalid hex digit in UUID";
}
}
