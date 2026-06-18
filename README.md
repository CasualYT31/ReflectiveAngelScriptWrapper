# ReflectiveAngelScriptWrapper
An experimental C++ wrapper for AngelScript that makes use of the recently-implemented reflection features of C++26.

This library's documentation assumes you are already familiar with AngelScript and its features. If you are not, then visit [the AngelScript website](https://www.angelcode.com/angelscript/).

The primary goal of this template library is to greatly reduce the registration code required to set up your application interface. Without it, you would have to register all of your classes, global variables, global functions, enums, etc., manually, which is error prone and a headache to maintain.

This wrapper does not aim to put AngelScript in a black box: you can directly access, for example, the engine object, if you need to.

## Table of Contents

0. [Prerequisites](#gcc-1610)
1. [Wrappers](/readmes/1-WRAPPERS.md)
    - [`SharedObject` and `OwnedObject`](/readmes/1-WRAPPERS.md#sharedobject-and-ownedobject)
    - [`Engine`](/readmes/1-WRAPPERS.md#engine)
2. [Typenames](/readmes/2-TYPENAMES.md)
    - [`TypeName`](/readmes/2-TYPENAMES.md#typename)
        - [Template Types](/readmes/2-TYPENAMES.md#template-types)
    - [`TmplTypeName`](/readmes/2-TYPENAMES.md#tmpltypename)
    - [`TypeOf`](/readmes/2-TYPENAMES.md#typeof)
3. [Properties](/readmes/3-PROPERTIES.md)
    - [`Engine::RegisterGlobalProperty()`](/readmes/3-PROPERTIES.md#engineregisterglobalproperty)
    - [Const Global Properties By Default](/readmes/3-PROPERTIES.md#const-global-properties-by-default)
4. [Functions](/readmes/4-FUNCTIONS.md)
    - [`Engine::RegisterGlobalFunction()`](/readmes/4-FUNCTIONS.md#engineregisterglobalfunction)
    - [Function Overloading](/readmes/4-FUNCTIONS.md#function-overloading)
    - [Call Convention](/readmes/4-FUNCTIONS.md#call-convention)
        - [Generic Call Convention](/readmes/4-FUNCTIONS.md#generic-call-convention)
    - [`GetFuncDecl()`](/readmes/4-FUNCTIONS.md#getfuncdecl)
        - [Value Types](/readmes/4-FUNCTIONS.md#value-types)
        - [Reference Types](/readmes/4-FUNCTIONS.md#reference-types)
        - [Default Values](/readmes/4-FUNCTIONS.md#default-values)
        - [Auto Handles](/readmes/4-FUNCTIONS.md#auto-handles)
        - [Variable Parameter Type](/readmes/4-FUNCTIONS.md#variable-parameter-type)
        - [Const Qualification](/readmes/4-FUNCTIONS.md#const-qualification)
    - [Variadic Arguments and Template Functions](/readmes/4-FUNCTIONS.md#variadic-arguments-and-template-functions)
5. [Funcdefs](/readmes/5-FUNCDEFS.md)
    - [`Engine::RegisterFuncdef()`](/readmes/5-FUNCDEFS.md#engineregisterfuncdef)
    - [`asIScriptFunction` Pointers and the `Funcdef` Annotation](/readmes/5-FUNCDEFS.md#asiscriptfunction-pointers-and-the-funcdef-annotation)
6. [Types](/readmes/6-TYPES.md)
    - [`RefType` Annotation](/readmes/6-TYPES.md#reftype-annotation)
7. [Typedefs](/readmes/7-TYPEDEFS.md)
8. [Enums](/readmes/8-ENUMS.md)
9. [Interfaces](/readmes/9-INTERFACES.md)
    - [`Engine::RegisterInterface()`](/readmes/9-INTERFACES.md#engineregisterinterface)
        - [Interface Inheritance](/readmes/9-INTERFACES.md#interface-inheritance)
        - [Multiple Inheritance](/readmes/9-INTERFACES.md#multiple-inheritance)
        - [What's Registered](/readmes/9-INTERFACES.md#whats-registered)
        - [C++ Namespaces Are Ignored](/readmes/9-INTERFACES.md#c-namespaces-are-ignored)
        - [The Same Interface is Never Registered More Than Once](/readmes/9-INTERFACES.md#the-same-interface-is-never-registered-more-than-once)
        - [Excluding Interfaces From Registration](/readmes/9-INTERFACES.md#excluding-interfaces-from-registration)

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
