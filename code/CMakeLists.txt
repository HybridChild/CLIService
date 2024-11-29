cmake_minimum_required(VERSION 3.20)
project(cliService VERSION 1.0)

enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest REQUIRED)
include(GoogleTest)

add_subdirectory(cliService)
add_subdirectory(test)
