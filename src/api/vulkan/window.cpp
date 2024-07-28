#include <fgla/window.hpp>
#include <fgla/context.hpp>
#include <fgla/debug.hpp>
#include <stdexcept>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace fgla {

struct Window::Handle {
    GLFWwindow *win;
};

Window::Window() {
    FGLA_DB_PRINTF("Window::Window\n");
    this->m_handle = new Handle{nullptr};

    fgla::Result res = fgla::initialize_context();
    if (res.status != fgla::Result::Status::SUCCESS) {
        throw std::runtime_error(res.desc);
    }
    FGLA_DB_PRINTF("GLFW ready\n");
    // no window creation here, that's left to higher-level constructors with arguments (see below)
    FGLA_DB_PRINTF("exit Window::Window\n");
}

Window::Window(uint32_t width, uint32_t height, std::string title) : Window() {
    FGLA_DB_PRINTF("Window::Window(width, height, title)\n");
    this->m_width = width;
    this->m_height = height;

    this->m_title = title;

        // TODO: fullscreen support
    this->m_handle->win = glfwCreateWindow(this->m_width, this->m_height, this->m_title.c_str(), nullptr, nullptr);

    if (!this->m_handle->win) {
        throw std::runtime_error("GLFW failed to create window");
    }
    FGLA_DB_PRINTF("Successfully created window (%u, %u, %s)\n", this->m_width, this->m_height, this->m_title.c_str());
    FGLA_DB_PRINTF("exit Window::Window\n");
}

void Window::update() {
    glfwPollEvents();
}

void *Window::handle() const {
    return this->m_handle->win;
}

bool Window::is_open() const {
    if (this->m_handle->win == nullptr) return false;
    return !glfwWindowShouldClose(this->m_handle->win);
}

Window::~Window() {
    FGLA_DB_PRINTF("Window::~Window\n");
    glfwDestroyWindow(this->m_handle->win);
    FGLA_DB_PRINTF("Destroyed GLFWwindow\n");

    delete this->m_handle;
    FGLA_DB_PRINTF("exit Window::~Window\n");
}

}