include(FetchContent)

set(PFX ${CMAKE_SOURCE_DIR}/src/agnostic)
set(FGLA_AGNOSTIC_SOURCES
	${PFX}/_.cpp
)

find_package(spdlog QUIET)

if(NOT spdlog_FOUND)
	message(STATUS "spdlog not found, building from source...")
	FetchContent_Declare(
		spdlog
		GIT_REPOSITORY https://github.com/gabime/spdlog.git
		GIT_TAG v1.x
	)

	FetchContent_MakeAvailable(spdlog)
else()
	message(STATUS "spdlog found")
endif()

set(FGLA_AGNOSTIC_LIBRARIES PUBLIC spdlog::spdlog)

unset(PFX)
