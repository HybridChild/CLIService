#!/bin/bash

if [ -d "out" ]; then
    rm -rf out || exit 1
fi
