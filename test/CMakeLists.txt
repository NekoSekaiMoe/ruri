cmake_minimum_required(VERSION 3.10)

project(test-root LANGUAGES CXX)

add_executable(test-root main.cpp)
add_compile_options(-g3)
add_compile_options(-O0)

if (VERSION)
    add_test(NAME ${PROJECT_NAME} COMMAND cd test; echo "run" | sudo gdb ./test-root)
else()
    add_test(NAME ${PROJECT_NAME} COMMAND echo "run" | sudo gdb ./test-root)
endif()

set(CTEST_OUTPUT_ON_FAILURE ON)

set_tests_properties(${PROJECT_NAME} PROPERTIES
    PASS_REGULAR_EXPRESSION ".*"
    ENVIRONMENT "CTEST_OUTPUT_ON_FAILURE=1"
)
