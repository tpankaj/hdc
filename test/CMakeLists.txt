enable_testing()
add_executable(mytest test_hdc.c unity.c)
add_test(myTest ./mytest)
