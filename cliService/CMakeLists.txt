cmake_minimum_required(VERSION 3.20)
project(CLIService 
        VERSION 1.0.0 
        LANGUAGES CXX)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Options
option(BUILD_TESTING "Build the testing tree" ON)

# Compiler flags
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
endif()

# Find required packages
find_package(GTest REQUIRED)

# Add subdirectories
add_subdirectory(lib)
add_subdirectory(example)

# Testing configuration
if(BUILD_TESTING)
  enable_testing()
  add_subdirectory(test)
endif()

# Configure CTest
if(BUILD_TESTING)
  include(CTest)
  include(GoogleTest)
  set(CTEST_PARALLEL_LEVEL 4)  # Matches VSCode setting
  set(CTEST_OUTPUT_ON_FAILURE ON)
  set(CTEST_COLOR_ON TRUE)
endif()
