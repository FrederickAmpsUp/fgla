find_package(slang CONFIG QUIET)

option(BUILD_SLANG "Always build Slang from source" OFF)

if (NOT slang_FOUND OR BUILD_SLANG)
    if (NOT BUILD_SLANG)
        message(WARNING "Slang not found, building from source!")
        message("It is highly recommended to install the Slang toolchain.")
    endif()

    include(FetchContent)

    FetchContent_Declare(
        slang
        GIT_REPOSITORY https://github.com/shader-slang/slang
        GIT_TAG        v2026.14.1
        GIT_SHALLOW    TRUE
    )

    FetchContent_MakeAvailable(slang)
else()
    message("System Slang installation found.")
endif()
