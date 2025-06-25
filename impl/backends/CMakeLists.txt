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

set(FGLA_BACKEND_TARGETS ${FGLA_BACKEND_TARGETS} PARENT_SCOPE)
