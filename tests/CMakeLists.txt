message(STATUS "Testing enabled.")

set(TEST_DIRS
    backend_registry
    instance
    adapter
    vulkan_queue_allocator
    device_queues
    ext_window_surface
    ext_window_surface_device
    ext_window_surface_device_image_view
)

foreach(test_name IN LISTS TEST_DIRS)
    set(test_exec test_${test_name})
    add_executable(${test_exec} ${test_name}/test.cpp)
    target_link_libraries(${test_exec} fgla::fgla magic_enum spdlog::spdlog)
    add_test(NAME ${test_exec} COMMAND ${test_exec})
endforeach()

enable_testing()
