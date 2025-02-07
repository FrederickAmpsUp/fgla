set(FGLA_TESTS
	test_setup
)

foreach(TEST IN LISTS FGLA_TESTS)
	add_executable(${TEST} ${TEST}.cpp)
	target_link_libraries(${TEST} fgla)
endforeach()
