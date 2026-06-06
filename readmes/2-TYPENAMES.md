# Typenames

This page documents the different ways you can convert C++ types into AngelScript type declarations.

## `TypeName`

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

## `TmplTypeName`

This library also supports rendering AngelScript template types and any of their specializations that you might need. Since the subtypes of template types in AngelScript are only known at runtime, though, they are handled slightly differently.

You need to call the `SubTypeList()` template function to generate a list of subtypes at compile time. Using `CScriptArray` as an example:

```cpp
static_assert(as::TmplTypeDecl<CScriptArray, as::SubTypeList<int>()> == "array<int>");
static_assert(as::TmplTypeDecl<CScriptArray, as::SubTypeList<const std::string>()> == "array<const string>");
static_assert(as::TmplTypeDecl<CScriptArray, as::SubTypeList<as::Tmpl<CScriptArray*, float>>()> == "array<array<float>@>");
static_assert(as::TmplTypeDecl<CScriptArray, as::SubTypeList<as::Tmpl<CScriptArray*, as::Tmpl<CScriptArray*, float>>>()> == "array<array<array<float>@>@>");
```

Notice how the `Tmpl` template struct can be used to nest template types as subtypes of other template types.

## `TypeOf`

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

---

<div align="center">
    <a href="1-WRAPPERS.md">◄ 1. Wrappers</a>
    &emsp;&emsp;
    <a href="/README.md#table-of-contents">Back to Contents</a>
    &emsp;&emsp;
    <a href="3-PROPERTIES.md">3. Properties ►</a>
</div>
