#!/bin/bash

HELP=0
CLEAN=0
TEST=0

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
    esac
done

if [ "$HELP" -eq "1" ]; then
    echo "    -c | --clean    Delete build folder before re-configuring"
    echo "    -t | --test     Build and run tests"
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

CXX="$GCC_BIN/g++-16.1.0" CC="$GCC_BIN/gcc-16.1.0" LD_LIBRARY_PATH="$GCC_LIB" cmake -B out
CXX="$GCC_BIN/g++-16.1.0" CC="$GCC_BIN/gcc-16.1.0" LD_LIBRARY_PATH="$GCC_LIB" cmake --build out --target $TARGETS
BUILD_RESULT=$?

if [ "$TEST" -eq "1" ]; then
    if [ "$BUILD_RESULT" -eq "0" ]; then
        LD_LIBRARY_PATH="$GCC_LIB" ./out/tests/ReflectiveAngelScriptWrapperTests
    else
        echo -e "\e[1;31mWill not run tests due to build failure!\033[0m"
    fi
fi
