message(STATUS "OpenGL backend enabled.")

add_library(fgla_backend_opengl STATIC instance.cpp backend.cpp)
target_link_libraries(fgla_backend_opengl PUBLIC tl::expected)
target_include_directories(fgla_backend_opengl PRIVATE ${PROJECT_SOURCE_DIR}/include/)
