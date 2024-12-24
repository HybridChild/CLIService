@echo off
setlocal
if "%1"=="" (
    set CONFIG=Debug
) else (
    set CONFIG=%1
)
pushd out\build || exit /b 1
ctest -C %CONFIG%
popd
endlocal
