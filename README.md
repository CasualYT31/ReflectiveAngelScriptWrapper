# ReflectiveAngelScriptWrapper
An experimental C++ wrapper for AngelScript that makes use of the recently-implemented reflection features of C++26.

This README assumes you are already familiar with AngelScript and its features. If you are not, then visit [the AngelScript website](https://www.angelcode.com/angelscript/).

The primary goal of this template library is to greatly reduce the registration code required to set up your application interface. Without it, you would have to register all of your classes, global variables, global functions, enums, etc., manually, which is error prone and a headache to maintain.

This wrapper does not aim to put AngelScript in a black box: you can directly access, for example, the engine object, if you need to.

## GCC 16.1.0

You will need to separately build GCC 16.1.0 and install it at the root of this repository within a `gcc-16.1.0` folder. In the future I want the `./set_up.sh` script to carry this out for you, but I can't get it to work for now, so use these as starting points until I can figure it out:

- [How to install gcc-14 on Ubuntu 22.04 and 24.04?](https://askubuntu.com/a/1518433).
- [Building gcc from source](https://d-meiser.github.io/2015/11/30/building-gcc-trunk.html).
- [Installing GCC](https://gcc.gnu.org/wiki/InstallingGCC).

## Scripts

**All scripts must be executed whilst the current working directory is the root of the repository!**

1. Execute `./set_up.sh`. This will install clang-format-18.
2. Execute `./build.sh`. This will configure and build the AngelScript wrapper library.
    - You can execute `./build.sh -c` if you need to perform a clean configure and build.
    - You can execute `./build.sh -t` to build and run the tests as well as the library.
    - Note the use of a pre-built version of GCC that supports C++26 features like reflection. It lives in `gcc-16.1.0`.

## Todo

1. Write examples as I write features and tests.
