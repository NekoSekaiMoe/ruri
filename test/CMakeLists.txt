cmake_minimum_required(VERSION 3.10)

project(ruri_test LANGUAGES CXX)

add_executable(test-root main.cpp)

enable_testing()

add_test(NAME ${PROJECT_NAME} COMMAND echo "run" | sudo gdb ./test-root)

set(CTEST_OUTPUT_ON_FAILURE ON)

set_tests_properties(${PROJECT_NAME} PROPERTIES
    PASS_REGULAR_EXPRESSION ".*"
    ENVIRONMENT "CTEST_OUTPUT_ON_FAILURE=1"
)
