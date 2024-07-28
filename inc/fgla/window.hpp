#pragma once

#include <cstdint>
#include <string>

namespace fgla {

class Window {
public:

    /**
     * @brief Constructor
     * Allocates resources for a window, but does not create one
     */
    Window();

    /**
     * @brief Constructor
     * Creates a window
     * 
     * @param width The width of the window
     * @param height The height of the window
     * @param title The title of the window, or "FGLA Window" if not provided
     */
    Window(uint32_t width, uint32_t height, std::string title="FGLA Window");

    /**
     * @brief Destructor
     * Destroys the window and frees all resources
     */
    ~Window();

    inline uint32_t width()  const { return m_width;  }
    inline uint32_t height() const { return m_height; }

    inline std::string title() const { return m_title; }

    /**
     * @brief Get the internal handle to the window
     * This generally should only be used internally
     */
    void *handle() const;

    /**
     * @brief Update the window
     * This runs all the tasks needed for a single frame of the window, such as event polling and framebuffer swaps
     */
    void update();

    /**
     * @brief Check if the window is still open
     * @retval true if the window is open, false otherwise
     */
    bool is_open() const;

private:
    uint32_t m_width, m_height;
    std::string m_title;

    struct Handle;
        // managed with RAII
    Handle *m_handle;
};
} // end namespace fgla