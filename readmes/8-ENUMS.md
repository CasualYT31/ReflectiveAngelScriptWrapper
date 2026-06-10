# Enums

Enums can also be registered via the `Engine` wrapper:

```cpp
// Unscoped enums are also supported.

enum class MyEnum {
    One,
    Two,
    Three,
    Four
}

engine.RegisterEnum<^^MyEnum>();

// ... is the same as ...

pEngine->RegisterEnum("MyEnum");
pEngine->RegisterEnumValue("MyEnum", "One", 0);
pEngine->RegisterEnumValue("MyEnum", "Two", 1);
pEngine->RegisterEnumValue("MyEnum", "Three", 2);
pEngine->RegisterEnumValue("MyEnum", "Four", 3);
```

Different types of enums are supported (e.g. enums with `std::uint8_t` values) , but note that enum values are cast to `int` as they are registered.

You can rename the enum (as well as individual values) in the script interface with the `Rename` annotation:

```cpp
enum class[[=as::Name("RenamedEnum")]] MyEnum {
    One = 40,
    Two,
    Three[[=as::Name("Third")]],
    Four = 60
}

engine.RegisterEnum<^^MyEnum>();

// ... is the same as ...

pEngine->RegisterEnum("RenamedEnum");
pEngine->RegisterEnumValue("RenamedEnum", "One", 40);
pEngine->RegisterEnumValue("RenamedEnum", "Two", 41);
pEngine->RegisterEnumValue("RenamedEnum", "Third", 42);
pEngine->RegisterEnumValue("RenamedEnum", "Four", 60);
```

---

<div align="center">
    <a href="7-TYPEDEFS.md">◄ 7. Typedefs</a>
    &emsp;&emsp;
    <a href="/README.md#table-of-contents">Back to Contents</a>
    &emsp;&emsp;
    <a href="9-INTERFACES.md">9. Interfaces ►</a>
</div>
