@echo off
if not exist "out" (
    mkdir out
    mkdir out\build
)
cmake -S .\code -B out\build
