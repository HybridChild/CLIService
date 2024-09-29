cmake_minimum_required(VERSION 3.20.1)

project(cliService)

add_executable(${PROJECT_NAME} main.cpp)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
