# Funcdefs

You can also register funcdefs.

## `Engine::RegisterFuncdef()`

Funcdefs are first declared in C++ like an ordinary function, except without a definition:

```cpp
// You can assign annotations to parameters like in regular functions,
// you just need to provide a parameter name to attach to.
void MyFuncdef(float, CScriptArray* arr[[=as::subtype::String]]);
```

You can then pass them to `RegisterFuncdef()`:

```cpp
engine.RegisterFuncdef<^^MyFuncdef>();

// ... is equivalent to ...

pEngine->RegisterFuncdef("void MyFuncdef(float, array<string>@)");
```

## `asIScriptFunction` Pointers and the `Funcdef` Annotation

Then, whenever you need to e.g. register a function that accepts a handle to a function that matches your funcdef, you can do the following:

```cpp
// Create the annotation for your funcdef so you don't need to construct it
// every time you want to use it.
constexpr Funcdef MyFuncdefHandle(^^MyFuncdef);

// All asIScriptFunction* parameters and objects _must_ have a Funcdef
// annotation attached to it so the library knows what type of function
// handle you want to work with.
void invokeCallback(asIScriptFunction* cb[[=MyFuncdefHandle]]) {
    // Invoke cb as you normally would.
}

engine.RegisterGlobalFunction<^^invokeCallback>();

// ... is equivalent to ...

pEngine->RegisterGlobalFunction(
    "void invokeCallback(MyFuncdef@)",
    asFUNCTION(invokeCallback),
    asCALL_CDECL
);
```

---

<div align="center">
    <a href="04-FUNCTIONS.md">◄ 04. Functions</a>
    &emsp;&emsp;
    <a href="/README.md#table-of-contents">Back to Contents</a>
    &emsp;&emsp;
    <a href="06-REFERENCE-TYPES.md">06. Reference Types ►</a>
</div>
