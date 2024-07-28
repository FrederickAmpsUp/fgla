#include <fgla/window.hpp>
#include <iostream>

int main(int argc, char **argv) {
    fgla::Window win(800, 600, "FGLA test window");

    while (win.is_open()) {
        win.update();
    }

    return 0;
}