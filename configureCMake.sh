#!/bin/bash

# Remove out/ directory if it exists
if [ -d "out" ]; then
    rm -rf out || exit 1
fi

mkdir -p out/build || exit 1

# Check for vcpkg toolchain file if VCPKG_ROOT is set
if [ -n "$VCPKG_ROOT" ]; then
    TOOLCHAIN_ARG="-DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
fi

cmake -S ./cliService -B out/build $TOOLCHAIN_ARG
