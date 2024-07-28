#include <fgla/window.hpp>
#include <fgla/instance.hpp>
#include <iostream>

int main(int argc, char **argv) {
    auto win = std::make_shared<fgla::Window>(800, 600, "FGLA test window");
    auto inst = std::make_shared<fgla::Instance>(win);

    while (win->is_open()) {
        win->update();
    }

    return 0;
}