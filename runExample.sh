#!/bin/bash

# Use Debug as default configuration if none provided
CONFIG=${1:-Debug}

./out/build/bin/"$CONFIG"/cliService_example
