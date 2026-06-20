# Interfaces

You can register interfaces to the application interface by simply providing a C++ equivalent to the `Engine::RegisterInterface()` template method.

## `Engine::RegisterInterface()`

Here is a simple example of registering an interface:

```cpp
struct MyInterface[[=as::Name("Renamed")]] {
    virtual void myMethod() = 0;
    virtual std::string generateString(int) const = 0;
    virtual std::uint64_t getSize(CScriptArray const* arr[[=as::subtype::Int32]]) const = 0;
};

engine->RegisterInterface<^^MyInterface>();

// ... is equivalent to ...

pEngine->RegisterInterface("Renamed");
pEngine->RegisterInterfaceMethod("Renamed", "void myMethod()");
pEngine->RegisterInterfaceMethod("Renamed", "string generateString(int32) const");
pEngine->RegisterInterfaceMethod("Renamed", "uint64 getSize(const array<int32>@) const");
```

Unfortunately, since interfaces are purely AngelScript constructs and cannot be directly mapped back to the C++ interface, you can't use the interface class when writing e.g. parameters for global functions, you instead need to use the `Interface` annotation:

```cpp
void functionThatAcceptsScriptObject(asIScriptObject* objThatImplementsMyInterface[[=as::Interface(^^MyInterface)]]) {
    // Call e.g. objThatImplementsMyInterface.myMethod() in the usual AngelScript fashion.
    // TODO: this fashion is very arduous! It might be nice to write template helper functions for this...
}
```

Then the library knows that you intend to receive an object from the scripts that is of a type—entirely defined within the scripts—that implements your interface.

### Interface Inheritance

But what about an interface hierarchy like this?

```cpp
struct InterfaceA {
    virtual void methodA() = 0;
};

struct InterfaceB : InterfaceA {
    virtual void methodB() = 0;
}

struct InterfaceC: InterfaceB {
    virtual void methodC() = 0;
}
```

What happens when you call `Engine::RegisterInterface<^^InterfaceC>()`?

When registered, `InterfaceC` will include all inherited methods for you. Not only that, but by default, `RegisterInterface()` will also recursively register each base interface as a separate interface! So the above call registers the same interfaces as the following AngelScript code:

```cpp
pEngine->RegisterInterface("InterfaceA");
pEngine->RegisterInterfaceMethod("InterfaceA", "void methodA()");

pEngine->RegisterInterface("InterfaceB");
pEngine->RegisterInterfaceMethod("InterfaceB", "void methodA()");
pEngine->RegisterInterfaceMethod("InterfaceB", "void methodB()");

pEngine->RegisterInterface("InterfaceC");
pEngine->RegisterInterfaceMethod("InterfaceC", "void methodA()");
pEngine->RegisterInterfaceMethod("InterfaceC", "void methodB()");
pEngine->RegisterInterfaceMethod("InterfaceC", "void methodC()");
```

If you don't want to register each base interface separately, you can instead call `Engine::RegisterInterface<^^InterfaceC, false>()`, which is the same as:

```cpp
pEngine->RegisterInterface("InterfaceC");
pEngine->RegisterInterfaceMethod("InterfaceC", "void methodA()");
pEngine->RegisterInterfaceMethod("InterfaceC", "void methodB()");
pEngine->RegisterInterfaceMethod("InterfaceC", "void methodC()");
```

> [!NOTE]
> It is not currently possible in AngelScript to define the relationships between these different interfaces. E.g. you cannot implicitly or explicitly cast an object handle to a type C that implements InterfaceC, to a handle to a type that implements InterfaceA.

### Multiple Inheritance

Multiple inheritance is also fully supported:

```cpp
struct DiamondTip {
    virtual void baseMethod() = 0;
}

struct DiamondLeft : DiamondTip {
    virtual void midMethod() = 0;
    virtual void leftMethod() = 0;
}

struct DiamondRight : DiamondTip {
    virtual void midMethod() = 0;
    virtual void rightMethod() = 0;
}

struct Diamond : DiamondLeft, DiamondRight {
    virtual void method() = 0;
}

engine.RegisterInterface<^^Diamond>();

// ... is equivalent to ...

pEngine->RegisterInterface("DiamondTip");
pEngine->RegisterInterfaceMethod("DiamondTip", "void baseMethod()");

pEngine->RegisterInterface("DiamondLeft");
pEngine->RegisterInterfaceMethod("DiamondLeft", "void baseMethod()");
pEngine->RegisterInterfaceMethod("DiamondLeft", "void midMethod()");
pEngine->RegisterInterfaceMethod("DiamondLeft", "void leftMethod()");

pEngine->RegisterInterface("DiamondRight");
pEngine->RegisterInterfaceMethod("DiamondRight", "void baseMethod()");
pEngine->RegisterInterfaceMethod("DiamondRight", "void midMethod()");
pEngine->RegisterInterfaceMethod("DiamondRight", "void rightMethod()");

pEngine->RegisterInterface("Diamond");
pEngine->RegisterInterfaceMethod("Diamond", "void baseMethod()");
pEngine->RegisterInterfaceMethod("Diamond", "void midMethod()");
pEngine->RegisterInterfaceMethod("Diamond", "void leftMethod()");
pEngine->RegisterInterfaceMethod("Diamond", "void rightMethod()");
pEngine->RegisterInterfaceMethod("Diamond", "void method()");
```

Notice how both `midMethod()`s from `DiamondLeft` and `DiamondRight` condense into a single `midMethod()` within `Diamond`, as they share the same signature.

> [!CAUTION]
> In reality, *this library does not make any effort to warn you about conflicts with methods defined in different base interfaces*, it will simply use the first version of the method it finds!

For example if `DiamondRight` declared `midMethod()` as returning `int` instead of `void`, `DiamondLeft`'s version of the method will be registered instead as the `int` version was found later during class traversal. Defining `Diamond` as `struct Diamond : DiamondRight : DiamondLeft { ... }` will have the opposite effect.

For this reason you should be careful when registering interfaces that use multiple inheritance.

Method overloading is also fully supported, though, so both versions of `midMethod()` would be included separately within `Diamond` if, for example, `DiamondRight::midMethod(int)` was declared instead.

### What's Registered

It's important to note that only public, pure virtual methods will be registered. The following, if found in the given C++ interface, will be ignored:

- Constructors and destructors.
- Operator functions, even if they are pure virtual.
- All protected and private members. This includes all members inherited via protected and private inheritance.
    - An interface inherited via `protected` or `private` will also not be registered in the script interface. You will need to register these separately.
- Concrete methods, and virtual methods that are not pure.
- Fields.

### C++ Namespaces Are Ignored

A warning on interfaces that share the same identifier in C++.

If you try to register interface `A` in this example:

```cpp
namespace one {
struct One {};
}

namespace two {
struct One : one::One {};
}

struct A : two::One {};
```

`RegisterInterface<^^A>()` will result in `asALREADY_REGISTERED`, as it tries to register both `One` interfaces with the same name.

To avoid this, you will need to do one of:

1. Rename one of the interfaces in C++.
2. Attach a `Rename` annotation to one of the interfaces like you can with other C++ entities.
3. Specialize TypeName for your interface.

```cpp
// Example of 2:

namespace one {
struct One {};
}

namespace two {
struct One[[=as::Name("OneRenamed")]] : one::One {};
}

struct A : two::One {};

engine.RegisterInterface<^^A>();

// ... is the same as ...

pEngine->RegisterInterface("A");
pEngine->RegisterInterface("OneRenamed");
pEngine->RegisterInterface("One");
```

### The Same Interface is Never Registered More Than Once

If you try to register the same interface more than once, either directly or indirectly, the wrapper will skip over registering it to prevent `asALREADY_REGISTERED` errors. This means you don't have to worry about accidentally registering the same base interface multiple times when registering all of its descendants (since, unfortunately, it's not possible for the wrapper to figure out derived interfaces, given a base interface).

### Excluding Interfaces From Registration

If you need to exclude interfaces from registration, then you can attach the `DoNotRegister` annotation to the interface:

```cpp
struct InterfaceA {
    virtual void methodA() = 0;
};

struct[[=as::DoNotRegister]] InterfaceB : InterfaceA {
    virtual void methodB() = 0;
}

struct InterfaceC: InterfaceB {
    virtual void methodC() = 0;
}

engine.RegisterInterface<^^InterfaceC>();

// ... is the same as ...

pEngine->RegisterInterface("InterfaceC");
pEngine->RegisterInterfaceMethod("InterfaceC", "void methodA()");
pEngine->RegisterInterfaceMethod("InterfaceC", "void methodB()");
pEngine->RegisterInterfaceMethod("InterfaceC", "void methodC()");
```

Notice that this also excludes all bases interfaces. But base interfaces that aren't annotated with `DoNotRegister` can still be registered separately.

You can also exclude individual methods from registration:

```cpp
struct InterfaceA {
    virtual void methodA() = 0;
};

struct InterfaceB : InterfaceA {
    virtual void methodB[[=as::DoNotRegister]]() = 0;
}

struct InterfaceC: InterfaceB {
    virtual void methodC() = 0;
}

engine.RegisterInterface<^^InterfaceC>();

// ... is the same as ...

pEngine->RegisterInterface("InterfaceA");
pEngine->RegisterInterfaceMethod("InterfaceA", "void methodA()");

pEngine->RegisterInterface("InterfaceB");
pEngine->RegisterInterfaceMethod("InterfaceB", "void methodA()");

pEngine->RegisterInterface("InterfaceC");
pEngine->RegisterInterfaceMethod("InterfaceC", "void methodA()");
pEngine->RegisterInterfaceMethod("InterfaceC", "void methodC()");
```

### Mixin Interfaces

Sometimes you might want to include some methods in an interface without treating the base interface they come from as a separate interface in the scripts. To do this, you can mark the interface as a mixin:

```cpp
struct BaseInterface {
    virtual void baseMethod() = 0;
};

struct[[=as::Mixin]] MyMixin : BaseInterface {
    virtual void commonMethod() = 0;
};

struct A : MyMixin {
    virtual void a() = 0;
};

engine.RegisterInterface<^^A>();

// ... is equivalent to ...

pEngine->RegisterInterface("A");
pEngine->RegisterInterfaceMethod("A", "void a()");
pEngine->RegisterInterfaceMethod("A", "void commonMethod()");
pEngine->RegisterInterfaceMethod("A", "void baseMethod()");

struct B : MyMixin, BaseInterface {
    virtual void b() = 0;
};

engine.RegisterInterface<^^B>();

// ... is equivalent to ...

pEngine->RegisterInterface("B");
pEngine->RegisterInterfaceMethod("B", "void b()");
pEngine->RegisterInterfaceMethod("B", "void commonMethod()");
pEngine->RegisterInterfaceMethod("B", "void baseMethod()");

pEngine->RegisterInterface("BaseInterface");
pEngine->RegisterInterfaceMethod("BaseInterface", "void baseMethod()");
```

Notice that all base interfaces of the mixin interface are also treated as mixins, but if those same base interfaces are registered or inherited separately, they will _not_ be treated as mixins.

You cannot register mixin interfaces directly.

---

<div align="center">
    <a href="08-ENUMS.md">◄ 08. Enums</a>
    &emsp;&emsp;
    <a href="/README.md#table-of-contents">Back to Contents</a>
    &emsp;&emsp;
    <a href="10-VALUE-TYPES.md">10. Value Types ►</a>
</div>
