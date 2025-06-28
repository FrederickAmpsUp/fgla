message(STATUS "Testing enabled.")

set(TEST_DIRS
    backend_registry
    instance
    adapter
    vulkan_queue_allocator
    device_queues
)

foreach(test_name IN LISTS TEST_DIRS)
    set(test_exec test_${test_name})
    add_executable(${test_exec} ${test_name}/test.cpp)
    target_link_libraries(${test_exec} fgla)
    add_test(NAME ${test_exec} COMMAND ${test_exec})
endforeach()

enable_testing()
