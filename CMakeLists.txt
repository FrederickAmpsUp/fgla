cmake_minimum_required(VERSION 3.14)
project(fgla
	DESCRIPTION "Graphics abstraction inspired by wgpu"
	VERSION 0.0.1
	LANGUAGES C CXX
)

set(FGLA_PLATFORM "vulkan" CACHE STRING "Pick the backend platform for fgla.")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT EXISTS ${CMAKE_SOURCE_DIR}/src/${FGLA_PLATFORM}.cmake)
	message(FATAL_ERROR "Invalid platform: \"${FGLA_PLATFORM}\"")
endif()

include(${CMAKE_SOURCE_DIR}/src/agnostic.cmake)
include(${CMAKE_SOURCE_DIR}/src/${FGLA_PLATFORM}.cmake)

add_library(fgla SHARED ${FGLA_AGNOSTIC_SOURCES} ${FGLA_PLATFORM_SOURCES})

target_include_directories(fgla PUBLIC ${CMAKE_SOURCE_DIR}/inc)
target_link_libraries(fgla PRIVATE ${FGLA_PLATFORM_LIBRARIES})
target_link_libraries(fgla ${FGLA_AGNOSTIC_LIBRARIES})

option(FGLA_BUILD_TESTS "Build test programs" ON)

if(FGLA_BUILD_TESTS)
	message(STATUS "Building fgla test programs")
	add_subdirectory(tests)
endif()
