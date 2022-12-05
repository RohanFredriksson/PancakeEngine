@echo off
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
echo Y | rmdir assets /s > nul
echo d | xcopy ..\assets assets /E > nul