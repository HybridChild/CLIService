@echo off
if not exist "out" (
    mkdir out
    mkdir out\build
)
cmake -S . -B out\build
