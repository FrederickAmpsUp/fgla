cmake_minimum_required(VERSION 3.10)
project(fgla)

include_directories(inc)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(spdlog REQUIRED)

file(GLOB AGNOSTIC_SRC "${CMAKE_SOURCE_DIR}/src/agnostic/*.cpp")

if(PLATFORM)
    set(PLATFORM_SRC "${CMAKE_SOURCE_DIR}/src/${PLATFORM}")
    if(EXISTS "${PLATFORM_SRC}")
        file(GLOB PLATFORM_FILES "${PLATFORM_SRC}/*.cpp")
        list(APPEND ALL_SRC ${PLATFORM_FILES})
    else()
	    message(WARNING "Platform directory '${PLATFORM_SRC}' does not exist. Proceeding without platform-specific files, this will likely fail.")
    endif()
else()
    message(FATAL_ERROR "No platform specified. Set the PLATFORM variable to a valid platform name.")
endif()

list(APPEND ALL_SRC ${AGNOSTIC_SRC})

add_library(fgla STATIC ${ALL_SRC})

# Link Vulkan and GLFW
target_link_libraries(fgla Vulkan::Vulkan glfw glm::glm spdlog::spdlog)

add_executable(test_app test/rolling.cpp)

target_link_libraries(test_app fgla)

set(CMAKE_CXX_STANDARD 17)

