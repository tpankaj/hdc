enable_testing()

add_executable(test_hdc_unit test_hdc_unit.c unity.c)
target_link_libraries(test_hdc_unit m)
add_test(test_hdc_unit ./test_hdc_unit)

add_executable(test_hdc_integration test_hdc_integration.c unity.c)
target_link_libraries(test_hdc_integration m)
add_test(test_hdc_integration ./test_hdc_integration)
