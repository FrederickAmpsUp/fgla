message(STATUS "Vulkan backend enabled.")

find_package(Vulkan REQUIRED)

if(NOT Vulkan_FOUND)
	message(FATAL_ERROR "Vulkan SDK not found, make sure it's installed and configured!")
endif()

add_library(fgla_backend_vulkan STATIC instance.cpp backend.cpp)
target_link_libraries(fgla_backend_vulkan PUBLIC tl::expected Vulkan::Vulkan)
target_include_directories(fgla_backend_vulkan PRIVATE ${PROJECT_SOURCE_DIR}/include/)
