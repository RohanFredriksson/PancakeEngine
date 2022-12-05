#!/bin/bash
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
rm -rf assets 2> /dev/null
cp -r ../assets assets