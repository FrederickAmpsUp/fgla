cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)

project(fgla
	VERSION 0.0.1
	LANGUAGES CXX)

set(EXPECTED_BUILD_TESTS OFF)

set(SPDLOG_BUILD_PIC ON)

add_subdirectory(${PROJECT_SOURCE_DIR}/extern/expected)
add_subdirectory(${PROJECT_SOURCE_DIR}/extern/spdlog)
add_subdirectory(${PROJECT_SOURCE_DIR}/extern/fmt)
add_subdirectory(impl)

option(FGLA_BUILD_TESTS "Build FGLA's test programs" ON)
if(FGLA_BUILD_TESTS)
	add_subdirectory(tests)
endif()
