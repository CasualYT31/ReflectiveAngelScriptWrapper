# ReflectiveAngelScriptWrapper
An experimental C++ wrapper for AngelScript that makes use of the recently-implemented reflection features of C++26.

This library's documentation assumes you are already familiar with AngelScript and its features. If you are not, then visit [the AngelScript website](https://www.angelcode.com/angelscript/).

The primary goal of this template library is to greatly reduce the registration code required to set up your application interface. Without it, you would have to register all of your classes, global variables, global functions, enums, etc., manually, which is error prone and a headache to maintain.

This wrapper does not aim to put AngelScript in a black box: you can directly access, for example, the engine object, if you need to.

## Table of Contents

0. [Prerequisites](#gcc-1610)
1. [Wrappers](/readmes/01-WRAPPERS.md)
    - [`SharedObject` and `OwnedObject`](/readmes/01-WRAPPERS.md#sharedobject-and-ownedobject)
    - [`Engine`](/readmes/01-WRAPPERS.md#engine)
2. [Typenames](/readmes/02-TYPENAMES.md)
    - [`TypeName`](/readmes/02-TYPENAMES.md#typename)
        - [Template Types](/readmes/02-TYPENAMES.md#template-types)
    - [`TmplTypeName`](/readmes/02-TYPENAMES.md#tmpltypename)
    - [`TypeOf`](/readmes/02-TYPENAMES.md#typeof)
3. [Properties](/readmes/03-PROPERTIES.md)
    - [`Engine::RegisterGlobalProperty()`](/readmes/03-PROPERTIES.md#engineregisterglobalproperty)
    - [Const Global Properties By Default](/readmes/03-PROPERTIES.md#const-global-properties-by-default)
4. [Functions](/readmes/04-FUNCTIONS.md)
    - [`Engine::RegisterGlobalFunction()`](/readmes/04-FUNCTIONS.md#engineregisterglobalfunction)
    - [Function Overloading](/readmes/04-FUNCTIONS.md#function-overloading)
    - [Call Convention](/readmes/04-FUNCTIONS.md#call-convention)
        - [Generic Call Convention](/readmes/04-FUNCTIONS.md#generic-call-convention)
    - [`GetFuncDecl()`](/readmes/04-FUNCTIONS.md#getfuncdecl)
        - [Value Types](/readmes/04-FUNCTIONS.md#value-types)
        - [Reference Types](/readmes/04-FUNCTIONS.md#reference-types)
        - [Default Values](/readmes/04-FUNCTIONS.md#default-values)
        - [Auto Handles](/readmes/04-FUNCTIONS.md#auto-handles)
        - [Variable Parameter Type](/readmes/04-FUNCTIONS.md#variable-parameter-type)
        - [Const Qualification](/readmes/04-FUNCTIONS.md#const-qualification)
    - [Variadic Arguments and Template Functions](/readmes/04-FUNCTIONS.md#variadic-arguments-and-template-functions)
5. [Funcdefs](/readmes/05-FUNCDEFS.md)
    - [`Engine::RegisterFuncdef()`](/readmes/05-FUNCDEFS.md#engineregisterfuncdef)
    - [`asIScriptFunction` Pointers and the `Funcdef` Annotation](/readmes/05-FUNCDEFS.md#asiscriptfunction-pointers-and-the-funcdef-annotation)
    - [Scoped Funcdefs](/readmes/05-FUNCDEFS.md#scoped-funcdefs)
6. [Reference Types](/readmes/06-REFERENCE-TYPES.md)
    - [`RefType` Annotation](/readmes/06-REFERENCE-TYPES.md#reftype-annotation)
    - [Basic Behaviors](/readmes/06-REFERENCE-TYPES.md#basic-behaviors)
        - [Factories](/readmes/06-REFERENCE-TYPES.md#factories)
            - [Factories With Auxiliary Objects](/readmes/06-REFERENCE-TYPES.md#factories-with-auxiliary-objects)
        - [AddRef, Release and `as::ReferenceType`](/readmes/06-REFERENCE-TYPES.md#addref-release-and-asreferencetype)
            - [`as::WeakReferenceType`](/readmes/06-REFERENCE-TYPES.md#asweakreferencetype)
    - [Naming Your Type](/readmes/06-REFERENCE-TYPES.md#naming-your-type)
    - [Methods](/readmes/06-REFERENCE-TYPES.md#methods)
    - [Properties](/readmes/06-REFERENCE-TYPES.md#properties)
    - [Property Accessors](/readmes/06-REFERENCE-TYPES.md#property-accessors)
    - [Composite Members](/readmes/06-REFERENCE-TYPES.md#composite-members)
    - [Exclusion](/readmes/06-REFERENCE-TYPES.md#exclusion)
    - [Inheritance](/readmes/06-REFERENCE-TYPES.md#inheritance)
7. [Typedefs](/readmes/07-TYPEDEFS.md)
8. [Enums](/readmes/08-ENUMS.md)
9. [Interfaces](/readmes/09-INTERFACES.md)
    - [`Engine::RegisterInterface()`](/readmes/09-INTERFACES.md#engineregisterinterface)
        - [Interface Inheritance](/readmes/09-INTERFACES.md#interface-inheritance)
        - [Multiple Inheritance](/readmes/09-INTERFACES.md#multiple-inheritance)
        - [What's Registered](/readmes/09-INTERFACES.md#whats-registered)
        - [C++ Namespaces Are Ignored](/readmes/09-INTERFACES.md#c-namespaces-are-ignored)
        - [The Same Interface is Never Registered More Than Once](/readmes/09-INTERFACES.md#the-same-interface-is-never-registered-more-than-once)
        - [Excluding Interfaces From Registration](/readmes/09-INTERFACES.md#excluding-interfaces-from-registration)
        - [Mixin Interfaces](/readmes/09-INTERFACES.md#mixin-interfaces)
10. [Value Types](/readmes/10-VALUE-TYPES.md)

## GCC 16.1.0

You will need to separately build GCC 16.1.0 and install it at the root of this repository within a `gcc-16.1.0` folder. In the future I want the `./set_up.sh` script to carry this out for you, but I can't get it to work for now, so use these as starting points until I can figure it out:

- [How to install gcc-14 on Ubuntu 22.04 and 24.04?](https://askubuntu.com/a/1518433).
- [Building gcc from source](https://d-meiser.github.io/2015/11/30/building-gcc-trunk.html).
- [Installing GCC](https://gcc.gnu.org/wiki/InstallingGCC).

## Scripts

**All scripts must be executed whilst the current working directory is the root of the repository!**

1. Execute `./set_up.sh`. This will install clang-format-18 (not required for the build).
2. Execute `./build.sh`. This will configure and build the AngelScript wrapper library.
    - You can execute `./build.sh -r` to remove debug symbols from the build.
    - You can execute `./build.sh -c` if you need to perform a clean configure and build.
    - You can execute `./build.sh -t` to build and run the tests as well as the library.
    - You can execute `./build.sh -d` to build and run the tests (via GDB) as well as the library.
    - Note the use of a pre-built version of GCC that supports C++26 features like reflection. It must live in a `gcc-16.1.0` folder at the root of the repository.
