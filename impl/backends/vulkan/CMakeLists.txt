message(STATUS "Vulkan backend enabled.")

find_package(Vulkan REQUIRED)
find_package(glfw3 3.3 QUIET)

add_subdirectory(
    ${CMAKE_SOURCE_DIR}/extern/VulkanMemoryAllocator
    ${CMAKE_BINARY_DIR}/extern/VulkanMemoryAllocator
)

if (NOT glfw3_FOUND)
  message(WARNING "GLFW3 not found, fgla::ext::windowing will not be supported!")
endif()

if(NOT Vulkan_FOUND)
	message(FATAL_ERROR "Vulkan SDK not found, make sure it's installed and configured!")
endif()

add_library(fgla_backend_vulkan STATIC
    instance.cpp
    adapter.cpp
    queue.cpp
    command_buffer/command_buffer.cpp
    command_buffer/buffer_image_copy.cpp
    command_buffer/image.cpp
    command_buffer/render_pass.cpp
    device/device.cpp
    device/queue.cpp
    device/shader.cpp
    device/pipeline.cpp
    device/buffer.cpp
    device/memory.cpp
    device/image.cpp
    render_pipeline.cpp
    shader_module.cpp
    backend.cpp
    image.cpp
    util.cpp
    render_pass.cpp
    memory.cpp
    buffer.cpp
    completion.cpp
    vma.cpp
)
target_link_libraries(fgla_backend_vulkan PRIVATE
    spdlog::spdlog fmt::fmt
    Vulkan::Vulkan slang::slang
    magic_enum GPUOpen::VulkanMemoryAllocator)

if (glfw3_FOUND)
  target_link_libraries(fgla_backend_vulkan PRIVATE glfw)
  target_compile_definitions(fgla_backend_vulkan PRIVATE FGLA_VK_EXT_WINDOWING)
  target_sources(fgla_backend_vulkan PRIVATE ext/windowing/window.cpp ext/windowing/surface.cpp)
endif()

target_include_directories(fgla_backend_vulkan PRIVATE ${PROJECT_SOURCE_DIR}/include/)
