@echo off
git submodule update --init --recursive
if not exist "build" mkdir build
cd build
if exist posix (
    del CMakeCache.txt /s /f /q > nul
) else (
    if not exist windows (
        del CMakeCache.txt /s /f /q > nul
    )
)
del posix /s /f /q > nul
type nul > windows
cmake .. -G "MinGW Makefiles"
mingw32-make