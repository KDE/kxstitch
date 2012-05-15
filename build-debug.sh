#!/bin/bash

rm -rf build
mkdir build
if [ -d "build" ]; then
    cd build
    THREADS=`cat /proc/cpuinfo | grep processor | wc -l`
    cmake -DCMAKE_INSTALL_PREFIX=`kde4-config --prefix` .. -DCMAKE_BUILD_TYPE=DebugFull && make -j${THREADS} && sudo make install
else
    echo "Unable to create build directory. Build aborted\n"
fi

