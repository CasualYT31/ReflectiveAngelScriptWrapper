#!/bin/bash

HELP=0
CLEAN=0
TEST=0
DEBUG=0
DEBUG_RUN_IMMEDIATELY=0

while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            HELP=1
            shift # past argument
            ;;
        -c|--clean)
            CLEAN=1
            shift # past argument
            ;;
        -t|--test)
            TEST=1
            shift # past argument
            ;;
        -d|--debug)
            TEST=1
            DEBUG=1
            shift # past argument
            ;;
        -dr|--debug-run)
            TEST=1
            DEBUG=1
            DEBUG_RUN_IMMEDIATELY=1
            shift # past argument
            ;;
    esac
done

if [ "$HELP" -eq "1" ]; then
    echo "    -c | --clean        Delete build folder before re-configuring"
    echo "    -t | --test         Build and run tests"
    echo "    -d | --debug        Build and run tests in GDB"
    echo "   -dr | --debug-run    Build and run tests in GDB, executing the tests immediately"
    exit
fi

if [ "$CLEAN" -eq "1" ]; then
    rm -rf out
fi

TARGETS=ReflectiveAngelScriptWrapper

if [ "$TEST" -eq "1" ]; then
    TARGETS="$TARGETS ReflectiveAngelScriptWrapperTests"
fi

GCC_BIN="$PWD/gcc-16.1.0/bin"
GCC_LIB="$PWD/gcc-16.1.0/lib64"

CXX_FLAGS=""
if [ "$DEBUG" -eq "1" ]; then
    CXX_FLAGS="-g"
fi

CXX="$GCC_BIN/g++-16.1.0" CC="$GCC_BIN/gcc-16.1.0" LD_LIBRARY_PATH="$GCC_LIB" cmake -B out -D CMAKE_CXX_FLAGS="$CXX_FLAGS"
CXX="$GCC_BIN/g++-16.1.0" CC="$GCC_BIN/gcc-16.1.0" LD_LIBRARY_PATH="$GCC_LIB" cmake --build out --target $TARGETS
BUILD_RESULT=$?

if [ "$TEST" -eq "1" ]; then
    if [ "$BUILD_RESULT" -eq "0" ]; then
        if [ "$DEBUG" -eq "1" ]; then
            if [ "$DEBUG_RUN_IMMEDIATELY" -eq "1" ]; then
                LD_LIBRARY_PATH="$GCC_LIB" gdb -ex run ./out/tests/ReflectiveAngelScriptWrapperTests
            else
                LD_LIBRARY_PATH="$GCC_LIB" gdb ./out/tests/ReflectiveAngelScriptWrapperTests
            fi
        else
            LD_LIBRARY_PATH="$GCC_LIB" ./out/tests/ReflectiveAngelScriptWrapperTests
        fi
    else
        echo -e "\e[1;31mWill not run tests due to build failure!\033[0m"
    fi
fi
