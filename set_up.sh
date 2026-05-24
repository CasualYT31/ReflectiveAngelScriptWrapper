#!/bin/bash

CLANG_FORMAT_VERSION=18

# TODO: I was able to build and include GCC 16.1.0 separately, but for some
#       reason I was not able to replicate my initial process in this script.
#       Instead of including the binaries, this script should be able to
#       download, extract, configure, and build GCC.

# Install clang-format-18 (https://askubuntu.com/a/1463201).
wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
sudo ./llvm.sh $CLANG_FORMAT_VERSION
rm llvm.sh
sudo apt install clang-format-$CLANG_FORMAT_VERSION
