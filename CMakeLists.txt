cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)

project(fgla
	VERSION 0.0.1
	LANGUAGES CXX)

set(EXPECTED_BUILD_TESTS OFF)
set(OPTIONAL_BUILD_TESTS OFF)

set(SPDLOG_BUILD_PIC ON)
set(SPDLOG_HEADER_ONLY OFF CACHE BOOL "" FORCE)

add_subdirectory(${PROJECT_SOURCE_DIR}/extern/spdlog)
add_subdirectory(${PROJECT_SOURCE_DIR}/extern/fmt)
add_subdirectory(${PROJECT_SOURCE_DIR}/extern/magic_enum)
add_subdirectory(impl)

add_library(fgla::fgla ALIAS fgla)

option(FGLA_BUILD_TESTS "Build FGLA's test programs" ON)
if(FGLA_BUILD_TESTS)
	add_subdirectory(tests)
endif()
