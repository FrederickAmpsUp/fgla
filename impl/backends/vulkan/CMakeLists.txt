message(STATUS "Vulkan backend enabled.")

add_library(fgla_backend_vulkan STATIC instance.cpp backend.cpp)
target_link_libraries(fgla_backend_vulkan PUBLIC tl::expected)
target_include_directories(fgla_backend_vulkan PRIVATE ${PROJECT_SOURCE_DIR}/include/)
