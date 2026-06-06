# Typedefs

Registering typedefs[^1] is as simple as providing the wrapper a reflection of your typedef in C++:

```cpp
using MyNumberTypedef = std::uint64_t

engine.RegisterTypedef<^^MyNumberTypedef>();

// ... is the same as ...

pEngine->RegisterTypedef("MyNumberTypedef", "uint64");
```

Note that when the wrapper, for example, registers a global property that uses a typedef, the typedef will _not_ be in the resulting declaration string due to type adjustment in C++. But this should not be a problem as the same underlying type will still be used.

You can also attach `Rename` annotations to typedefs like you can with most other C++ entities, but make sure that you provide the typedef with the annotation attached to it and not a typedef to that typedef:

```cpp
// Both "using" and "typedef" variants are supported:

typedef bool MyFlag[[=as::Name("MyRename")]];

typedef MyFlag MyTypedef;

engine.RegisterTypedef<^^MyTypedef>();

// ... is the same as ...

pEngine->RegisterTypedef("MyTypedef", "bool");

engine.RegisterTypedef<^^MyFlag>();

// ... is the same as ...

pEngine->RegisterTypedef("MyRename", "bool");
```

The `decl` parameter that is given to the underlying `RegisterTypedef()` call will be the recursively dealiased type's AngelScript-equivalent typename. The typedef's name in the application interface will _only_ be based on the first typedef in the chain, as you can see above.

---

<div align="center">
    <a href="6-TYPES.md">◄ 6. Types</a>
    &emsp;&emsp;
    <a href="/README.md#table-of-contents">Back to Contents</a>
    &emsp;&emsp;
    <a href="8-ENUMS.md">8. Enums ►</a>
</div>

[^1]: Reminder that [AngelScript only supports registering typedefs for primitive types](https://www.angelcode.com/angelscript/sdk/docs/manual/classas_i_script_engine.html#addb24466769dc52be96c7e37d5305245).
