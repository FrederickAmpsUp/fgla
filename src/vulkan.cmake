set(PFX  ${CMAKE_SOURCE_DIR}/src/vulkan)

set(FGLA_PLATFORM_SOURCES
	${PFX}/instance.cpp
	${PFX}/window.cpp
)

find_package(glfw3 3.3 REQUIRED)
find_package(Vulkan REQUIRED)

set(FGLA_PLATFORM_LIBRARIES glfw Vulkan::Vulkan)

unset(PFX)
