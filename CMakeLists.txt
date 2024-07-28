cmake_minimum_required(VERSION 3.10)
project(FGLA)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

message(STATUS "Using C++17")

# Include directory
include_directories(inc)

# Count the number of header files
file(GLOB_RECURSE HEADER_FILES
     "inc/fgla/*.h"
     "inc/fgla/*.hpp")

list(LENGTH HEADER_FILES HEADER_FILE_COUNT)

function(count_lines_in_file file output)
    file(READ ${file} CONTENT)
    string(REPLACE ";" "" CONTENT ${CONTENT})
    string(REPLACE "\n" ";" CONTENT_LIST ${CONTENT})
    list(LENGTH CONTENT_LIST LINE_COUNT)
    set(${output} ${LINE_COUNT} PARENT_SCOPE)
endfunction()

# Aggregate line counts
set(TOTAL_LINES 0)
foreach(file ${HEADER_FILES})
    count_lines_in_file(${file} LINE_COUNT)
    math(EXPR TOTAL_LINES "${TOTAL_LINES} + ${LINE_COUNT}")
endforeach()

message(STATUS "Include path inc/, ${HEADER_FILE_COUNT} FGLA headers found totalling ${TOTAL_LINES} lines.")

link_directories(lib/)

if(NOT DEFINED FGLA_API_BACKEND)
# Set a default value if FGLA_API_BACKEND is not defined
set(FGLA_API_BACKEND "vulkan" CACHE STRING "Backend API to use for FGLA")
endif()

# Print the value of FGLA_API_BACKEND for verification
message(STATUS "FGLA_API_BACKEND is set to ${FGLA_API_BACKEND}")

# Gather all source files from src/ recursively
file(GLOB_RECURSE SRC_FILES src/agnostic/*.c src/agnostic/*.cpp src/api/${FGLA_API_BACKEND}/*.c src/api/${FGLA_API_BACKEND}/*.cpp)

list(LENGTH SRC_FILES SRC_FILE_COUNT)

# Aggregate line counts
set(TOTAL_LINES 0)
foreach(file ${SRC_FILES})
    count_lines_in_file(${file} LINE_COUNT)
    math(EXPR TOTAL_LINES "${TOTAL_LINES} + ${LINE_COUNT}")
endforeach()

message(STATUS "Source path src/, ${SRC_FILE_COUNT} files found totalling ${TOTAL_LINES} lines.")

# Create a static library from the gathered source files
add_library(fgla STATIC ${SRC_FILES})

find_package(glfw3 3.3 REQUIRED)
target_link_libraries(fgla glfw vulkan)

# Gather all source files from test/ (non-recursive)
file(GLOB TEST_FILES test/*.c test/*.cpp)

# Create executables for each source file in test/
foreach(TEST_FILE ${TEST_FILES})
    # Get the filename without extension
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    # Create the executable
    add_executable(${TEST_NAME} ${TEST_FILE})
    # Link the executable with the fglwDeps library
    target_link_libraries(${TEST_NAME} fgla)
endforeach()

# Gather all source files from run/ (non-recursive)
file(GLOB RUN_FILES run/*.c run/*.cpp)

# Create executables for each source file in run/
foreach(RUN_FILE ${RUN_FILES})
    # Get the filename without extension
    get_filename_component(RUN_NAME ${RUN_FILE} NAME_WE)
    # Create the executable
    add_executable(${RUN_NAME} ${RUN_FILE})
    # Link the executable with the fglwDeps library
    target_link_libraries(${RUN_NAME} fgla)
endforeach()

# Install the run executables
install(TARGETS ${RUN_NAME} DESTINATION /usr/bin)