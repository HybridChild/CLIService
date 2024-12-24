#!/bin/bash

# Use Debug as default configuration if none provided
CONFIG=${1:-Debug}

cmake --build out/build --config "$CONFIG" --parallel "$(nproc)"
