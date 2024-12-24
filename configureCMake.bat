@echo off
setlocal
if not exist "out" (
    mkdir out || exit /b 1
)
if not exist "out\build" (
    mkdir out\build || exit /b 1
)

:: Check if VCPKG_ROOT is set, otherwise use default path
if "%VCPKG_ROOT%"=="" (
    set VCPKG_TOOLCHAIN=c:/tools/vcpkg/scripts/buildsystems/vcpkg.cmake
) else (
    set VCPKG_TOOLCHAIN=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake
)

cmake -S ./cliService -B out\build -DCMAKE_TOOLCHAIN_FILE="%VCPKG_TOOLCHAIN%"
endlocal
