#!/bin/bash

# Use Debug as default configuration if none provided
CONFIG=${1:-Debug}

cd out/build || exit 1
ctest -C "$CONFIG"
cd ../..
