#include <fgla/util.hpp>

std::string fgla::format_memory_size(size_t bytes) {
    const char* units[] = {"B", "KiB", "MiB", "GiB", "TiB"};
    double size = static_cast<double>(bytes);
    int unit_index = 0;

    while (size >= 1024 && unit_index < 4) {
        size /= 1024;
        unit_index++;
    }

    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << size << " " << units[unit_index];
    return ss.str();
}
