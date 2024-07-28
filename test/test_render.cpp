#include <fgla/window.hpp>
#include <fgla/instance.hpp>
//#include <fgla/shader.hpp>
#include <iostream>
#include <fstream>
#include <vector>

std::vector<char> load_file(const std::string filename) {
    std::vector<char> result;
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    result.resize(file.tellg());
    file.seekg(0);

    file.read(result.data(), result.size());

    file.close();

    return result;
}

int main(int argc, char **argv) {
    auto win = std::make_shared<fgla::Window>(800, 600, "FGLA test window");
    auto inst = std::make_shared<fgla::Instance>(win);

    auto shader = fgla::whatever;
    auto renderer = fgla::whatever2(inst, shader, layout);
    auto geometry = fgla::whatever3(layout);
    geometry.add_triangle(a, b, c);

    while (win->is_open()) {
        renderer->render(geometry);

        win->update();
    }

    return 0;
}