# cliService/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(cliService VERSION 1.0)

# Instead, set the testing output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(UNITTEST_OUTPUT_DIR ${CMAKE_BINARY_DIR}/Testing)

# Create the testing directory
file(MAKE_DIRECTORY ${UNITTEST_OUTPUT_DIR})

enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest REQUIRED)
include(GoogleTest)

# Set the working directory for all tests to the test output directory
set(CMAKE_CTEST_ARGUMENTS "--output-on-failure" "--test-output-size-passed=65536" "--test-output-size-failed=65536")
set(CMAKE_CTEST_COMMAND_ARGS "--test-dir ${UNITTEST_OUTPUT_DIR}")

add_subdirectory(lib)
add_subdirectory(test)
add_subdirectory(example)
