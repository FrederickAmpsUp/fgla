message(STATUS "Vulkan backend enabled.")

find_package(Vulkan REQUIRED)
find_package(glfw3 3.3 QUIET)

if (NOT glfw3_FOUND)
  message(WARNING "GLFW3 not found, fgla::ext::windowing will not be supported!")
endif()

if(NOT Vulkan_FOUND)
	message(FATAL_ERROR "Vulkan SDK not found, make sure it's installed and configured!")
endif()

add_library(fgla_backend_vulkan STATIC instance.cpp adapter.cpp queue.cpp device.cpp backend.cpp)
target_link_libraries(fgla_backend_vulkan PRIVATE tl::expected spdlog::spdlog Vulkan::Vulkan)
if (glfw3_FOUND)
  target_link_libraries(fgla_backend_vulkan PRIVATE glfw)
  target_compile_definitions(fgla_backend_vulkan PRIVATE FGLA_VK_EXT_WINDOWING)
  target_sources(fgla_backend_vulkan PRIVATE ext/window.cpp)
endif()
target_include_directories(fgla_backend_vulkan PRIVATE ${PROJECT_SOURCE_DIR}/include/)
