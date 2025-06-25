include(${PROJECT_SOURCE_DIR}/cmake/modules/force_link.cmake)

set(FGLA_BACKEND_TARGETS "")

function(add_fgla_backend NAME)
    option(FGLA_BACKEND_${NAME} "Enable the FGLA ${NAME} backend" ON)
    if(FGLA_BACKEND_${NAME})
        add_subdirectory(${NAME})
        list(APPEND FGLA_BACKEND_TARGETS fgla_backend_${NAME})
    endif()
    set(FGLA_BACKEND_TARGETS "${FGLA_BACKEND_TARGETS}" PARENT_SCOPE)
endfunction()

add_fgla_backend(opengl)
add_fgla_backend(vulkan)

add_library(fgla_backend_all INTERFACE)

foreach(backend IN LISTS FGLA_BACKEND_TARGETS)
	fgla_force_link_backend(fgla_backend_all INTERFACE ${backend})
endforeach()
