@echo off
if not exist "out" (
    mkdir out
    mkdir out\build
)
cmake -S ./CLIService -B out\build -DCMAKE_TOOLCHAIN_FILE=c:/tools/vcpkg/scripts/buildsystems/vcpkg.cmake
