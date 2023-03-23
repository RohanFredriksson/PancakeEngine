#!/bin/bash
git submodule update --init --recursive
mkdir build 2> /dev/null
cd build
if [ -f windows ]; then
    rm CMakeCache.txt
elif [ ! -f posix ]; then
    rm CMakeCache.txt
fi
rm windows 2> /dev/null
touch posix
cmake ..
make