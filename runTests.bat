@echo off
pushd out\build || exit /b 1
ctest
popd
