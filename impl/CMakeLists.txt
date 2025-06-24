add_subdirectory(backends)

add_library(fgla_glue instance.cpp)
target_include_directories(fgla_glue PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(fgla_glue PUBLIC tl::expected fmt::fmt)

add_library(fgla INTERFACE)
target_include_directories(fgla INTERFACE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(fgla INTERFACE fgla_backend_all fgla_glue)
