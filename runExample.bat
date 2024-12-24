@echo off
setlocal
if "%1"=="" (
    set CONFIG=Debug
) else (
    set CONFIG=%1
)
.\out\build\bin\%CONFIG%\cliService_example.exe
endlocal
