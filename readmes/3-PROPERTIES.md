# Properties

A less impressive use case for the `Engine` wrapper is the ability to register global properties without needing to handwrite the property declaration string.

## `Engine::RegisterGlobalProperty()`

The following pairs of statements are identical:

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

## Const Global Properties By Default

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

---

<div align="center">
    <a href="2-TYPENAMES.md">◄ 2. Typenames</a>
    &emsp;&emsp;
    <a href="/README.md#table-of-contents">Back to Contents</a>
    &emsp;&emsp;
    <a href="4-FUNCTIONS.md">4. Functions ►</a>
</div>
