@echo off
setlocal
if "%1"=="" (
    set CONFIG=Debug
) else (
    set CONFIG=%1
)
cmake --build out\build --config %CONFIG% --parallel %NUMBER_OF_PROCESSORS%
endlocal
