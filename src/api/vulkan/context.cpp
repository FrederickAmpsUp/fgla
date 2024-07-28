#include <fgla/context.hpp>
#include <GLFW/glfw3.h>

fgla::Result fgla::initialize_context() {
        // have we already initialized?
    static bool ready = false;

    if (!ready) {
        ready = true;
        if (!glfwInit()) {
            return fgla::Result{
                fgla::Result::Status::GL_ERROR,
                "GLFW initialization failed"
            };
        }
        
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    }

    return fgla::Result{
        fgla::Result::Status::SUCCESS,
    };
}