# Wrappers

This page describes wrapper classes that help you interact with the AngelScript library.

## `SharedObject` and `OwnedObject`

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

## `Engine`

Likewise, there are two ways you can create an `Engine` wrapper/helper object:

```cpp
// Either keep the scripting engine entirely encapsulated...
as::Engine engine;
// ...with the occasional access when required for advanced operations.
engine.Ptr()->GarbageCollect();
// The wrapper will release the scripting engine when it's destructed.

// Or create the scripting engine yourself...
::asIScriptEngine* const pEngine = asCreateScriptEngine();
// ...and provide it to a wrapper whenever it makes sense to do so.
as::Engine engine(pEngine);
// The wrapper will release its reference to the engine when it's destructed...
pEngine->Release();
// ...meaning you will need to manage the engine's release yourself.
```

---

<div align="center">
    &emsp;&emsp;
    <a href="/README.md#table-of-contents">Back to Contents</a>
    &emsp;&emsp;
    <a href="02-TYPENAMES.md">02. Typenames ►</a>
</div>
