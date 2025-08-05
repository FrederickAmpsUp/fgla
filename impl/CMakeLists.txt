add_subdirectory(backends)

include(${PROJECT_SOURCE_DIR}/cmake/modules/force_link.cmake)

add_library(fgla_glue instance.cpp)
target_include_directories(fgla_glue PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(fgla_glue PUBLIC fmt::fmt)

add_library(fgla INTERFACE)
target_include_directories(fgla INTERFACE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(fgla INTERFACE fgla_glue)
foreach(backend IN LISTS FGLA_BACKEND_TARGETS)
	fgla_force_link_backend(fgla INTERFACE ${backend})
endforeach()
