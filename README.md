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

### `SharedObject` and `OwnedObject`

These are convenience classes that wrap around AngelScript objects. They handle reference counting for you in much the same way `std::shared_ptr` and `std::unique_ptr` do.

The main difference between the two is `OwnedObject` assumes ownership of the objects you give it, meaning it won't increment the object's reference counter, whilst `SharedObject` does not assume ownership and so it will increment the object's reference counter when it receives the object.

Here are equivalent examples showcasing the differences between these classes:

```cpp
{
    const auto pEngine = asCreateScriptEngine();
    // The engine's reference counter is 1.

    as::SharedObject engine(pEngine);
    // The engine's reference counter is now 2!

    pEngine->Release();
    // The engine's reference counter is now 1.
    // pEngine should no longer be used.

    const auto count = engine->GetGlobalFunctionCount();
    // Notice that the SharedObject wrapper still holds a reference!
}

// That reference has now been dropped and the engine has been destructed.
```

```cpp
{
    as::OwnedObject engine(asCreateScriptEngine());
    // engine now owns the asIScriptEngine pointer.
    // asCreateScriptEngine() initializes the engine's reference counter
    // to 1, and its result is moved straight into the wrapper object.
}

// engine's destructor decrements the reference counter, which triggers the
// underlying asCScriptEngine destructor since the counter has reached 0.
```

The `Object` wrapper classes perfectly illustrate the two approaches you can take when working with this library: either you retain as much control over the lifetime of AngelScript's objects as possible, or you can surrender that control to RAII classes.

### `Engine`

Likewise, there are two ways you can create an Engine wrapper/helper object:

```cpp
// Either keep the scripting engine entirely encapsulated...
as::Engine engine;
// ...with the occasional access when required for advanced operations.
engine.engine()->GarbageCollect();
// The wrapper will release the scripting engine when it's destructed.

// Or create the scripting engine yourself...
::asIScriptEngine* const pEngine = asCreateScriptEngine();
// ...and provide it to a wrapper whenever it makes sense to do so.
as::Engine engine(pEngine);
// The wrapper will release its reference to the engine when it's destructed...
pEngine->Release();
// ...meaning you will need to manage the engine's release yourself.
```

### `TypeName`

The fundamental building block of this wrapper is the `TypeName` template variable and its specializations. `Engine` uses this template variable to deduce what the AngelScript equivalent typename is (in `constexpr std::string_view` form) for a given C++ typename. This library defines specializations for all of the primitive types, and also all of the types registered by add ons. It can also detect const qualifiers and whether or not a typename is a pointer, and include "const" and "@" symbols where required. If a C++ typename doesn't have a specialization for this function, `std::meta` will be used to deduce the typename's identifier and stringize it directly. See [TypeDecl.tpp](/include/AngelScriptWrapper/TypeDecl.tpp) for `TypeName` specialization examples.

You can of course access `TypeName` yourself, but usually the `Engine` wrapper will deal with accessing it for you.

```cpp
static_assert(as::TypeDecl<void> == "void");
static_assert(as::TypeDecl<bool> == "bool");
static_assert(as::TypeDecl<int> == "int32");
static_assert(as::TypeDecl<std::uint64_t> == "uint64");
static_assert(as::TypeDecl<std::string> == "string");
static_assert(as::TypeDecl<MyCustomType> == "MyCustomType");
```

### `TmplTypeName`

This library also supports rendering AngelScript template types and any of their specializations that you might need. Since the subtypes of template types in AngelScript are only known at runtime, though, they are handled slightly differently.

You need to call the `SubTypeList()` template function to generate a list of subtypes at compile time. Using `CScriptArray` as an example:

```cpp
static_assert(as::TmplTypeDecl<CScriptArray, as::SubTypeList<int>()> == "array<int>");
static_assert(as::TmplTypeDecl<CScriptArray, as::SubTypeList<const std::string>()> == "array<const string>");
static_assert(as::TmplTypeDecl<CScriptArray, as::SubTypeList<as::Tmpl<CScriptArray*, float>>()> == "array<array<float>@>");
static_assert(as::TmplTypeDecl<CScriptArray, as::SubTypeList<as::Tmpl<CScriptArray*, as::Tmpl<CScriptArray*, float>>>()> == "array<array<array<float>@>@>");
```

Notice how the `Tmpl` template struct can be used to nest template types as subtypes of other template types.

### `TypeOf`

You can also generate the equivalent AngelScript type of C++ entities such as variables, fields, and functions:

```cpp
bool flag;
std::string myMessage;
const CScriptArray* const arr[[=as::SubTypeList<double>()]];
const CScriptArray* const unknownSubtypes;

static_assert(as::TypeOf<^^flag> == "bool");
static_assert(as::TypeOf<^^myMessage> == "string");
static_assert(as::TypeOf<^^arr> == "const array<double>@ const");
static_assert(as::TypeOf<^^unknownSubtypes> == "const array@ const");

// Doing this with functions will generate the AngelScript declaration of the return type:
std::string func(int number);
static_assert(as::TypeOf<^^func> == "string");
```

Again, since the subtypes of template types are known only at runtime, you need to attach annotation to your C++ objects to tell the library which subtypes you expect to use with them. If you neglect to attach a subtype list, the library will default to the base typename of the AngelScript template type.

### `Engine::RegisterGlobalProperty()`

A less impressive use case for the `Engine` wrapper is the ability to register global properties without needing to handwrite the property declaration string. The following pairs of statements are identical:

```cpp
as::Engine engine;
asIScriptEngine* pEngine = engine.Ptr();

int myGlobal = 56;

engine.RegisterGlobalProperty<^^myGlobal>(&myGlobal);
pEngine->RegisterGlobalProperty("int myGlobal", &myGlobal);

// AngelScript does not like registering pointers to const objects in this context,
// so we'll need to turn the const-ness of the property into an annotation.
int myConstGlobal[[=as::GlobalConst]] = 56;

engine.RegisterGlobalProperty<^^myConstGlobal>(&myConstGlobal);
pEngine->RegisterGlobalProperty("const int myConstGlobal", &myConstGlobal);

// Template types work like non-template types, provided you attach a SubTypes
// annotation to the C++ object!
CScriptArray* globalArray[[=as::SubTypeList<std::string>()]];

engine.RegisterGlobalProperty<^^globalArray>(&globalArray);
pEngine->RegisterGlobalProperty("array<string>@ GlobalArray", &globalArray);

// If you want to name your property something different in AngelScript,
// you will need to attach a Rename annotation to the C++ object:
bool flag[[=as::Name("CustomFlag")]] = true;

engine.RegisterGlobalProperty<^^flag>(&flag);
pEngine->RegisterGlobalProperty("bool CustomFlag", &flag);

// There is a special version of the method that works with OwnedObjects:
as::OwnedObject<CScriptArray> ownedArray[[
    // There are a variety of common-used subtype lists
    // defined within the as::subtype namespace.
    =as::subtype::String,
    =as::Name("RenamedArray")
]](globalArray);

engine.RegisterGlobalProperty<^^ownedArray>(ownedArray);
pEngine->RegisterGlobalProperty("array<string> RenamedArray", ownedArray.Ptr());
```
