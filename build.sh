#!/bin/bash

BUILD_TYPE="Release"
WITH_PROFILE=""
VERBOSE=""
SILENCE_DEPRECATED="-DSILENCE_DEPRECATED=1"
THREADS=`cat /proc/cpuinfo | grep processor | wc -l`

readopt='getopts $opts opt;rc=$?;[ $rc$opt == 0? ]&&exit 1;[ $rc == 0 ]||{ shift $[OPTIND-1];false; }'
opts=dhpsvn
while eval $readopt
do
    if [ $opt == "d" ]
    then
        BUILD_TYPE="DebugFull"
    fi

    if [ $opt == "h" ]
    then
        SHOW_HELP=true
    fi

    if [ $opt == "p" ]
    then
        WITH_PROFILE="-DWITH_PROFILING=On"
    fi

    if [ $opt == "s" ]
    then
        THREADS=1
    fi

    if [ $opt == "v" ]
    then
        VERBOSE="VERBOSE=1"
    fi

    if [ $opt == "n" ]
    then
        SILENCE_DEPRECATED=""
    fi
done

# Turn off profiling if compiling with debug
if [ $BUILD_TYPE == "DebugFull" ]
then
    WITH_PROFILE=""
fi

if (${SHOW_HELP:=false})
then
    echo "Usage build.sh -dhpsvn"
    echo "  -d : Build with debugging enabled"
    echo "  -h : Show this help"
    echo "  -p : Build with profiling enabled"
    echo "  -s : Build with single thread"
    echo "  -v : Build with verbose compiler output"
    echo "  -n : No silencing of deprecated declarations"
else
    rm -rf build
    mkdir build
    if [ -d "build" ]; then
        cd build
        cmake -DCMAKE_INSTALL_PREFIX=`qtpaths --install-prefix` .. -DCMAKE_BUILD_TYPE=$BUILD_TYPE $WITH_PROFILE $SILENCE_DEPRECATED && make -j${THREADS} $VERBOSE && sudo make install
    else
        echo "Unable to create build directory. Build aborted\n"
    fi
fi
