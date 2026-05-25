# ReflectiveAngelScriptWrapper
An experimental C++ wrapper for AngelScript that makes use of the recently-implemented reflection features of C++26.

This README assumes you are already familiar with AngelScript and its features. If you are not, then visit [the AngelScript website](https://www.angelcode.com/angelscript/).

The primary goal of this template library is to greatly reduce the registration code required to set up your application interface. Without it, you would have to register all of your classes, global variables, global functions, enums, etc., manually, which is error prone and a headache to maintain.

This wrapper does not aim to put AngelScript in a black box: you can directly access, for example, the engine object, if you need to.

[Click here to skip ahead to the examples.](#examples)

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

## Examples

### `Object` and `OwnedObject`

These are convenience classes that wrap around AngelScript objects. They handle reference counting for you in much the same way `std::shared_ptr` and `std::unique_ptr` do.

The main difference between the two is `OwnedObject` assumes ownership of the objects you give it, meaning it won't increment the object's reference counter, whilst `Object` does not assume ownership and so it will increment the object's reference counter when it receives the object.

Here are equivalent examples showcasing the differences between these classes:

```cpp
{
    const auto pEngine = ::asCreateScriptEngine();
    // The engine's reference counter is 1.

    as::Object engine(pEngine);
    // The engine's reference counter is now 2!

    pEngine->Release();
    // The engine's reference counter is now 1.
    // pEngine should no longer be used.

    const auto count = engine->GetGlobalFunctionCount();
    // Notice that the Object wrapped still holds a reference!
}

// That reference has now been dropped and the engine has been destructed.
```

```cpp
{
    as::OwnedObject engine(::asCreateScriptEngine());
    // engine now owns the asIScriptEngine pointer.
    // asCreateScriptEngine() initializes the engine's reference counter
    // to 1, and its result is moved straight into the wrapper object.
}

// engine's destructor decrements the reference counter, which triggers the
// underlying asCScriptEngine destructor since the counter has reached 0.
```
