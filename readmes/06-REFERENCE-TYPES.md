# Reference Types

This README documents the process of registering reference types via the `Engine` wrapper. If you want to register value types, please read [this document](/readmes/10-VALUE-TYPES.md) instead.

## `RefType` Annotation

C++ types are by default registered as value types. If you want to register a type as being a reference type, you must annotate it with `RefType`.

## Basic Behaviors

Reference types must implement some combination of Factory, AddRef and Release behaviors, or none at all, depending on your use case:

Factory? | AddRef? | Release? | Category of Reference Type
--- | --- | --- | ---
Yes | Yes | Yes | [Standard reference type with reference counting](https://www.angelcode.com/angelscript/sdk/docs/manual/doc_reg_basicref.html)
Yes | No | No | [Standard reference type without reference counting](https://www.angelcode.com/angelscript/sdk/docs/manual/doc_reg_basicref.html#doc_reg_nocount)
Yes | No | Yes | [Scoped reference type](https://www.angelcode.com/angelscript/sdk/docs/manual/doc_adv_scoped_type.html) <br /> (TODO: not yet supported by the wrapper!)
No | Yes | Yes | [Uninstantiable reference type with reference counting](https://www.angelcode.com/angelscript/sdk/docs/manual/doc_reg_basicref.html#doc_reg_noinst)
No | No | No | [Single-reference type](https://www.angelcode.com/angelscript/sdk/docs/manual/doc_adv_single_ref_type.html)
No | No | Yes | [Uninstantiable scoped reference type](https://www.angelcode.com/angelscript/sdk/docs/manual/doc_adv_scoped_type.html) <br /> (TODO: not yet supported by the wrapper!)

### Factories

Factories are added to reference types by declaring a public static method in the type and attaching the `as::Behaviour(asBEHAVE_FACTORY)` annotation to it.

It is also good practise to attach `as::NonAuto` to the factory, especially if your type initializes its reference counter to 1 upon construction outside of the factory. That way, if you ever need to make auto handles the default, turning the setting on during later stages of development will not cause all of your factories to initialize objects with a reference count of 2!

The following is an example of a default factory. Note that one is not generated for you, which means that reference types are by default single-reference or are otherwise not instantiable if you don't write one!

```cpp
struct[[=as::RefType]] MyReferenceType {
    static inline MyReferenceType* MyDefaultFactory[[
        =as::Behaviour(asBEHAVE_FACTORY),
        =as::NonAuto
    ]]() {
        return new MyReferenceType();
    }
};
```

It is also important to note that factory functions are not inherited. This means each class in your hierarchy must define their own factory/ies if you want each of them to be instantiable.

#### Factories With Auxiliary Objects

If you want to pass an auxiliary object to a factory method, you must do the following:

1. Add the auxiliary object to the factory's parameter list at either the start or end.
2. Attach `as::ObjFirst` or `as::ObjLast` to the factory method depending on where you put the auxiliary parameter.
3. Use `as::AuxLabel()` to attach an `as::AuxiliaryLabel` annotation to the factory method with a label unique to the object.
4. Call `Engine::AddAuxiliaryObject()`, giving it the label you gave to `as::AuxLabel()`. This must be carried out before register the object type!
5. Register the object type as normal.

Here is an example:

```cpp
struct MyHelper;

// It is nice practise to construct the AuxiliaryLabel once and reuse it.
constexpr auto MyHelperLabel = as::AuxLabel("MyHelper");

struct[[=as::RefType]] MyReferenceType {
    static inline MyReferenceType* MyAuxFactory[[
        =as::Behaviour(asBEHAVE_FACTORY),
        =as::NonAuto,
        =as::ObjFirst,
        =MyHelperLabel
    ]](MyHelper* helper, bool otherParam) {
        helper->DoSomething(otherParam);

        return new MyReferenceType();
    }

    static inline MyReferenceType* MyAuxFactory[[
        =as::Behaviour(asBEHAVE_FACTORY),
        =as::NonAuto,
        =as::ObjLast,
        =MyHelperLabel
    ]](bool otherParam, float additionalParam, MyHelper* helper) {
        helper->DoSomethingElse(otherParam, additionalParam);

        return new MyReferenceType();
    }
};

MyHelper helperObject;

engine.AddAuxiliaryObject(MyHelperLabel, &helperObject);
engine.RegisterObjectType<^^MyReferenceType>();

// ... is the same as ...

pEngine->RegisterObjectType("MyReferenceType", 0, asOBJ_REF || asOBJ_NOCOUNT);

pEngine->RegisterObjectBehaviour(
    "MyReferenceType",
    asBEHAVE_FACTORY,
    // Reminder that auxiliary objects aren't included in the signature:
    "MyReferenceType@ MyAuxFactory(bool)",
    asFUNCTIONPR(MyReferenceType::MyAuxFactory, (MyHelper*, bool), MyReferenceType*),
    asCALL_CDECL_OBJFIRST,
    &helperObject
);

pEngine->RegisterObjectBehaviour(
    "MyReferenceType",
    asBEHAVE_FACTORY,
    "MyReferenceType@ MyAuxFactory(bool, float)",
    asFUNCTIONPR(MyReferenceType::MyAuxFactory, (bool, float, MyHelper*), MyReferenceType*),
    asCALL_CDECL_OBJLAST,
    &helperObject
);
```

### AddRef, Release, and `as::ReferenceType`

These behaviors are added as non-static methods and annotated with `as::Behaviour(asBEHAVE_ADDREF)` or `as::Behaviour(asBEHAVE_RELEASE)` respectively.

However, since reference counting logic usually remains the same across all reference-counted types, a base `ReferenceType` mixin class is provided for you to inherit from. It initializes the reference counter to 1 for you, and when it hits 0, your object will be deleted via `delete this`. And unlike factories, AddRef and Release behaviors are inherited. Just don't forget to make the destructor of your reference types virtual if you intend to inherit from them.

Therefore, to create a standard reference-counted type with a default factory, it is as simple as:

```cpp
struct[[=as::RefType]] StandardType : public as::ReferenceType {
    static inline StandardType* Factory[[
        =as::Behaviour(asBEHAVE_FACTORY),
        =as::NonAuto
    ]]() {
        return new StandardType();
    }

    // Uncomment if you want to derive from StandardType!
    // virtual ~StandardType() noexcept = default;
};

engine->RegisterObjectType<^^StandardType>();

// Without the wrapper, you would have to implement StandardType in the same way
// (minus the annotations), but you'd also have to manually register everything:

pEngine->RegisterObjectType("StandardType", 0, asOBJ_REF);

pEngine->RegisterObjectBehaviour(
    "StandardType",
    asBEHAVE_ADDREF,
    "void AddRef() const",
    asMETHOD(StandardType, AddRef),
    asCALL_THISCALL
);

pEngine->RegisterObjectBehaviour(
    "StandardType",
    asBEHAVE_RELEASE,
    "void Release() const",
    asMETHOD(StandardType, Release),
    asCALL_THISCALL
);

pEngine->RegisterObjectBehaviour(
    "StandardType",
    asBEHAVE_FACTORY,
    "StandardType@ Factory()",
    asFUNCTION(StandardType::Factory),
    asCALL_CDECL
);
```

If you don't wish to use reference counting, then you can just avoid inheriting from `ReferenceType`.

## Naming Your Type

By default, the identifier of your class is used as the name of your type in AngelScript. If you wish to change it you will need to either [specialize `TypeName`](/readmes/02-TYPENAMES.md#typename) or use the `as::Name()` function to attach an `as::Rename` annotation to the type.

All C++ entities registered with the application interface via the `Engine` wrapper can elect to be called something different in the AngelScript engine using the `Rename` annotation. You use the `as::Name()` function to generate one, like so:

```cpp
struct[[=as::RefType, =as::Name("RenamedType")]] StandardType : public as::ReferenceType {
    // Etc.
};

engine->RegisterObjectType<^^StandardType>();

// ... is the same as ...

pEngine->RegisterObjectType("RenamedType", 0, asOBJ_REF);

// Etc.
```

For C++ template types, you are required to specialize `TypeName`, as each specialization of your template type will be registered as completely separate classes in the application interface, and they can't all have the same name.

## Methods

All public methods that aren't registered as behaviours will be registered as callable methods of the object type. This includes static methods, so make sure that you use the [`ObjFirst` and `ObjLast` annotation](/readmes/04-FUNCTIONS.md#call-convention) appropriately on those.

## Properties

All public fields will be registered as properties of the object type. This includes handling for pointer fields, i.e. [indirect fields](https://www.angelcode.com/angelscript/sdk/docs/manual/doc_reg_objprop.html).

## Property Accessors

To register property accessors on an object type, you will need to prepend the method's name with either `get_` or `set_`, as it is also required by AngelScript, and you will also have to attach the `as::PropertyAccessor` annotation to the method:

```cpp
struct[[=as::RefType]] StandardType : public as::ReferenceType {
    // Etc.

    inline int get_myProperty[[=as::PropertyAccessor]]() const {
        return m_myProperty;
    }

    inline void set_myProperty[[=as::PropertyAccessor]](int newProp) {
        m_myProperty = newProp;
    }

private:
    int m_myProperty = 0;
};

engine->RegisterObjectType<^^StandardType>();

// ... is the same as ...

pEngine->RegisterObjectType("StandardType", 0, asOBJ_REF);

// Etc.

pEngine->RegisterObjectMethod(
    "StandardType",
    "int32 get_myProperty() const property",
    asMETHOD(StandardType, get_myProperty),
    asCALL_THISCALL
);

pEngine->RegisterObjectMethod(
    "StandardType",
    "void set_myProperty(int32) property",
    asMETHOD(StandardType, set_myProperty),
    asCALL_THISCALL
);
```

[Indexed property accessors](https://www.angelcode.com/angelscript/sdk/docs/manual/doc_reg_objprop.html#doc_reg_objprop_accessor) work in a similar fashion, simply add the index parameter as required and the wrapper will handle it for you.

## Composite Members

Note that both composite methods and composite properties are currently not supported.

## Exclusion

You can exclude certain members in [the same way you can for interfaces](/readmes/09-INTERFACES.md#excluding-interfaces-from-registration).

## Inheritance

Class hierarchies are also registered for object types in the exact same way they are for [interfaces](/readmes/09-INTERFACES.md#interface-inheritance). This includes inheriting all base members automatically, and [mixin behavior](/readmes/09-INTERFACES.md#mixin-interfaces). But since AngelScript reference types can be directly mapped back to their C++ counterparts, the wrapper establishes class relationships (i.e. [registers the casting operators](https://www.angelcode.com/angelscript/sdk/docs/manual/doc_adv_class_hierarchy.html)) for you, as well, thus saving you a lot of duplicate code!

Both explicit and implicit casts are implemented as simple `dynamic_cast`s under the hood.

If you do not wish to register base classes of a given reference type (including their relationships), you can add `false` to the list of template parameters to `RegisterObjectType()` in the same way you can for `RegisterInterface()`.

---

<div align="center">
    <a href="05-FUNCDEFS.md">◄ 05. Funcdefs</a>
    &emsp;&emsp;
    <a href="/README.md#table-of-contents">Back to Contents</a>
    &emsp;&emsp;
    <a href="07-TYPEDEFS.md">07. Typedefs ►</a>
</div>
