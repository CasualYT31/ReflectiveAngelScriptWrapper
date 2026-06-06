# ReflectiveAngelScriptWrapper
An experimental C++ wrapper for AngelScript that makes use of the recently-implemented reflection features of C++26.

This README assumes you are already familiar with AngelScript and its features. If you are not, then visit [the AngelScript website](https://www.angelcode.com/angelscript/).

The primary goal of this template library is to greatly reduce the registration code required to set up your application interface. Without it, you would have to register all of your classes, global variables, global functions, enums, etc., manually, which is error prone and a headache to maintain.

This wrapper does not aim to put AngelScript in a black box: you can directly access, for example, the engine object, if you need to.

## Table of Contents

0. [Prerequisites](#gcc-1610)
1. [Wrappers](/readmes/1-WRAPPERS.md)
    - [`SharedObject` and `OwnedObject`](/readmes/1-WRAPPERS.md#sharedobject-and-ownedobject)
    - [`Engine`](/readmes/1-WRAPPERS.md#engine)
2. [Typenames](/readmes/2-TYPENAMES.md)

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

#### Const Global Properties By Default

If you want to make global properties const by default, you can set an `Engine` option. `Engine` has a list of compile-time options provided as a template argument to the `Engine` type. You can find a full list of them in the `EngineOptions` [struct definition](/include/AngelScriptWrapper/Engine.hpp). The option you'll be interested in is `ConstGlobalPropertiesDefault`, which defaults to `false`.

```cpp
as::Engine<{ .ConstGlobalPropertiesDefault = true }> engine;
asIScriptEngine* pEngine = engine.Ptr();

// Now global properties will be registered as const by default:
double counter = 0.0;
engine.RegisterGlobalProperty<^^counter>(&counter);
pEngine->RegisterGlobalProperty("const double counter", &counter);

// There is an annotation for removing the const qualifier should you need it:
float nonConstCounter[[=as::GlobalNotConst]] = 0.0;
engine.RegisterGlobalProperty<^^nonConstCounter>(&nonConstCounter);
pEngine->RegisterGlobalProperty("float nonConstCounter", &nonConstCounter);
```

### `Engine::RegisterGlobalFunction()`

You can do a lot more with this wrapper, though. For instance, you can also register C++ functions as global functions in the application interface by simply giving the wrapper a reflection of them!

```cpp
// Standard CDecl function:

std::int32_t add(const std::int32_t a, const std::int32_t b) {
    return a + b;
}

engine.RegisterGlobalFunction<^^add>();

// The above is equivalent to:
pEngine->RegisterGlobalFunction(
    "int32 add(int32, int32)", //*
    asFUNCTION(add),
    asCALL_CDECL
);

// * The const qualification of non-ref and non-ptr parameters is lost,
//   this is standard C++ behavior and the declaration is effectively
//   the same in any case.

// Static class CDecl function:

class MyClass {
public:
    // You can rename functions in the application interface in the same way
    // you can global properties.
    static double multiply[[=as::Name("mult")]](double a, double b, double c) {
        return a * b * c;
    }
};

engine.RegisterGlobalFunction<^^MyClass::multiply>();

// The above is equivalent to:
pEngine->RegisterGlobalFunction(
    "double mult(double, double, double)",
    asFUNCTION(MyClass::multiply),
    asCALL_CDECL
);

// Non-static class method:

struct MyStruct {
    bool isTrue(bool const& a) const {
        return a;
    }
} myStructInstance;

engine.RegisterGlobalFunction<^^MyStruct::isTrue>(&myStructInstance);

// The above is equivalent to:
pEngine->RegisterGlobalFunction(
    "bool isTrue(const bool&in)",
    asMETHOD(MyStruct, isTrue),
    asCALL_THISCALL_ASGLOBAL,
    &myStructInstance
);
```

Notice how you will no longer need to:
- handwrite the function declaration in AngelScript (in 99% of cases: see [later](#generic-call-convention) for the only exception),
- use any of the AngelScript macros to provide the function pointer, or
- specify the call convention to use (in most cases: again, see [later](#call-convention)).

All of this information can be reflected upon by the library and deduced for you.

#### Function Overloading

You will notice that when you try to provide the reflection of an overloaded function, the compiler will produce an error. This is because it won't know which version of the function you intend to register:

```cpp
std::uint64_t total(std::uint64_t a, std::uint64_t b) {
    return a + b;
}

double total(double a, double b) {
    return a + b;
}

// Which total should be registered/reflected here?
engine.RegisterGlobalFunction<^^total>();
```

For these cases you will need to use the `FindOverload()` helper function:

```cpp
// Since FindOverload is consteval, its result needs
// to be stored in a separate constexpr:
constexpr auto dblTotal = as::FindOverload<^^::, double, false, double, double>(
    "total"
);
engine.RegisterGlobalFunction<dblTotal>();

// This is the equivalent of the following in pure AngelScript:
pEngine->RegisterGlobalFunction(
    "double total(double, double)",
    asFUNCTIONPR(total, (double, double), double),
    asCALL_CDECL
);
```

- `^^::` is a reflection of the global namespace, where the `total`s are defined. If you needed to find the overload of a class method, you would provide a reflection of the class that the method lives within. And if your overloaded function is within a namespace, you'd provide the reflection of the namespace.
- The first `double` is the return type of the function you're looking for.
- `false` tells `FindOverload` if the method is const-qualified. For functions, this will always be `false`.
- Then each function parameter is provided, from first to last.

#### Call Convention

AngelScript currently supports the following call conventions:

- CDecl, with `OBJFIRST` and `OBJLAST` variants where applicable.
- StdCall, without the aforementioned variants.
- ThisCall, with said variants, and a special `ASGLOBAL` variant for `RegisterGlobalFunction()`.
- A generic call convention bespoke to AngelScript.

So, how is this library able to determine which call convention to use?

Some cases are very easy. For example, ThisCall will always be used for non-static class methods, so if one is found during reflection, that call convention is chosen, and the `ASGLOBAL` variant is used by `RegisterGlobalFunction()` for you.

Sadly, it is mostly not possible to detect other types of call convention via reflection, so annotations were also used for this purpose:

```cpp
// Attach the StdCall annotation to tell the library the function needs to be
// registered with the asCALL_STDCALL convention.
void stdCall[[=as::StdCall]]() {}

// CDecl is the default, but the library has no way of knowing if you intend
// a parameter to be `OBJFIRST` or `OBJLAST`, so annotations were also added
// so you can tell the library your intention:
void cdecl[[=as::ObjFirst]](MyObj* o, MyObj* o) {}
void cdecl[[=as::ObjLast]](MyObj* o, MyObj* o) {}
```

Like with [const global properties](#const-global-properties-by-default), you can set a `CallConventionDefault` to make it something other than CDecl if you'd prefer. It can be set to one of `asCALL_CDECL`, `asCALL_STDCALL`, or `asCALL_GENERIC`. There exists an `as::CDecl` annotation for the case where your default is StdCall but some of your functions use CDecl.

##### Generic Call Convention

The generic call convention is a particularly tricky case as the usual way you write functions that follow it makes it impossible to generate a declaration using traditional means. It is therefore impossible to avoid having to write the declaration for these functions:

```cpp
void generic[[as::Generic("int myGenericFunction(array<string>@, int64)")]](asIScriptGeneric* g);
```

However, with the help of the autowrapper add on, it is possible for you to write most functions as normal whilst using the generic call convention _and_ letting the library generate the declaration for you!

```cpp
int myGenericFunction[[as::Generic()]](CScriptArray* arr[[=as::subtype::String]], std::int64_t b);
```

This is what happens behind the scenes when you set the default call convention to generic. However, if you write the function with the special `void(asIScriptGeneric*)` signature, then you will **always** have to use the `as::Generic()` function to attach a `GenericWithDecl` annotation that contains the function declaration to register with, even if the default call convention is set to generic.

Additionally, you can attach `as::ObjFirst` and `as::ObjLast` to functions that use the generic call convention to tell the library [which autowrapper macro to use](https://www.angelcode.com/angelscript/sdk/docs/manual/doc_addon_autowrap.html).

Another option is to write a separate C++ declaration for your generic function and then pass that in to the template version of the `as::Generic<>()` helper function:

```cpp
constexpr EngineOptions opts = { .AutoHandleDefault = true };

int myGenericFunction(CScriptArray* arr[[=as::subtype::String]], std::int64_t);

void generic[[as::Generic<^^myGenericFunction, opts.AutoHandleDefault>()]](asIScriptGeneric* g);

// This will then register the function with the name "myGenericFunction":
engine.RegisterGlobalFunction<^^generic>();
```

Note that you can also pass in the `AutoHandleDefault` and `RC` parameters used by `GetFuncDecl()`. You will not need to mess around with `RC` 99.9% of the time, but `AutoHandleDefault` is usually populated with the `EngineOptions::AutoHandleDefault` option when it is used within `Engine::RegisterGlobalFunction()`, so the best approach would be to define the `EngineOptions` constexpr struct separately and use its value in all calls to `as::Generic<>()`, instead of giving the options to your `Engine` instance in-place.

#### `GetFuncDecl()`

The mechanism behind function declaration generation can be found in this template function. Here are tables describing how it converts C++ parameters to AngelScript equivalents. Keep in mind that C++ types are by default values types: they must be annotated with [`RefType`](#reftype-annotation) to be considered reference types.

##### Value Types

Description | C++ | AngelScript | Notes
--- | --- | --- | ---
By value | `int` | `int32` |
By const | `const int` | `int32` | Type adjustment in C++ means `const` in this case is lost (though there is still technically logic for this in `GetFuncDecl()`).
By reference | `int&` | `int32&out` | Value types cannot be passed as `&inout`.
By const reference | `int const&` | `const int32&in` |
By pointer | `int*` | `int32&out` | Value types cannot be passed as `&inout`.
By pointer to const | `int const*` | `const int32&in` | Const pointers (i.e. `int* const`) don't change the type used.

Return types that are value types are converted into their corresponding AngelScript declaration using regular [`TypeOf()`](#typeof) logic, where the function's reflection object is given as the template argument.

##### Reference Types

Description | C++ | AngelScript | Notes
--- | --- | --- | ---
By value | `CScriptArray` |  | Not supported!
By const | `const CScriptArray` |  | Not supported!
By reference | `CScriptArray&` | `array<T>&` | Don't forget to attach a subtype list via annotation as mentioned previously to deduce `T`.
By const reference | `CScriptArray const&` | `const array<T>&` |
By pointer | `CScriptArray*` | `array<T>@` |
By pointer to const | `CScriptArray const*` | `const array<T>@` |

Return types that are reference types use the same parameter logic as can be found in the above table.

##### Default Values

If you want your AngelScript function declaration to include default values, you can use the `DefVal()` template function to attach a `DefaultsTo` annotation to a parameter:

```cpp
void f(bool myFlag, int defaultValueOnlyInAngelScript[[=as::DefVal("5")]], std::string const& defaultValueInBoth[[=as::DefVal("\"Hi\"")]] = "Hi");

static_assert(as::GetFuncDecl<^^f>() == "void f(bool, int32 = 5, string = \"Hi\")");
```

Notice that the default value is always given as a string to `DefVal`, so make sure to surround any string literals with quotes.

##### Auto Handles

The `as::Auto` annotation can be attached to object handle parameters to turn them into an [auto handle](https://www.angelcode.com/angelscript/sdk/docs/manual/doc_obj_handle.html#doc_obj_handle_4). There is another `Engine` option, `AutoHandleDefault`, which can be set to `true` to make all handle parameters auto handles by default, with the `as::NonAuto` annotation used to explicitly mark a handle parameter as not an auto handle.

##### Variable Parameter Type

`void*` and `const void*` parameters are _always_ treated as the beginning of a [variable parameter type](https://www.angelcode.com/angelscript/sdk/docs/manual/doc_adv_var_type.html) declaration, meaning they must _always_ be followed by an `int` parameter which will store the type ID of the AngelScript value given.

```cpp
void e(void* ref, int typeId);
void f(const void* ref, int typeId, int separateParam);

static_assert(as::GetFuncDecl<^^e>() == "void e(?&out)");
static_assert(as::GetFuncDecl<^^f>() == "void f(const ?&in, int32)");
```

##### Const Qualification

If the function itself is a non-static class method with the `const` qualifier, the AngelScript declaration will also be appended with the `const` qualifier. There is a template parameter in `GetFuncDecl()` which can be used to strip this if it exists. This is used by `Engine::RegisterGlobalFunction()`.

#### Variadic Arguments and Template Functions

These are "supported," although since they both require the generic call convention, the `as::GenericWithDecl` annotation must be used with a full AngelScript function declaration, so this library won't help you in particular here.

### `Engine::RegisterObjectType()`

TODO: write once implementation is finished. But before that, I should transfer the entire Examples section to a Wiki.

#### `RefType` Annotation

C++ types are by default registered as value types. If you want to register a type as being a reference type, you must annotate it with `RefType`. See [`TypeDecl.tpp`](/include/AngelScriptWrapper/TypeDecl.tpp) for an example of how this annotation is used to tell this library that types such as `CScriptArray` are reference types.
