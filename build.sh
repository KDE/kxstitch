#!/bin/bash

rm -rf build
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=`kde4-config --prefix` .. && make && su -c 'make install'

