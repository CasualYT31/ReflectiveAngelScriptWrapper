#pragma once

#include <aswrappedcall.h>

namespace as {
template <EngineOptions Opts>
template <std::meta::info V, typename T>
    requires(!IsConst<T>)
int Engine<Opts>::RegisterGlobalProperty(T* const value) {
    if (!HasEngine()) { return AS_NAMESPACE_QUALIFIER asINVALID_ARG; }
    constexpr auto vName = as::GetIdentifierOf<V>();
    std::string name = std::string(vName);
    if constexpr (IsGlobalConstQualified<V, Opts.ConstGlobalPropertiesDefault>()) {
        constexpr auto constType = detail::OverrideTypeOf<const T, V>;
        name = std::string(constType) + " " + name;
    } else {
        constexpr auto type = detail::OverrideTypeOf<T, V>;
        name = std::string(type) + " " + name;
    }
    return Ptr()->RegisterGlobalProperty(name.c_str(), value);
}

template <EngineOptions Opts>
template <std::meta::info V, typename T>
    requires(!IsConst<T>)
int Engine<Opts>::RegisterGlobalProperty(OwnedObject<T> const& value) {
    if (!HasEngine()) { return AS_NAMESPACE_QUALIFIER asINVALID_ARG; }
    constexpr auto vName = as::GetIdentifierOf<V>();
    std::string name = std::string(vName);
    if constexpr (IsGlobalConstQualified<V, Opts.ConstGlobalPropertiesDefault>()) {
        constexpr auto constType = detail::OverrideTypeOf<const T, V>;
        name = std::string(constType) + " " + name;
    } else {
        constexpr auto type = detail::OverrideTypeOf<T, V>;
        name = std::string(type) + " " + name;
    }
    return Ptr()->RegisterGlobalProperty(name.c_str(), value.Ptr());
}

template <EngineOptions Opts> template <std::meta::info F> int Engine<Opts>::RegisterGlobalFunction(void* auxiliary) {
    AS_NAMESPACE_QUALIFIER asDWORD cc;
    return RegisterGlobalFunction<F>(cc, auxiliary);
}

template <EngineOptions Opts>
template <std::meta::info F>
int Engine<Opts>::RegisterGlobalFunction(AS_NAMESPACE_QUALIFIER asDWORD& callConvOut, void* auxiliary) {
    if (!HasEngine()) { return AS_NAMESPACE_QUALIFIER asINVALID_ARG; }
    constexpr auto deducedCallConv = FuncCallConv<F, Opts.CallConventionDefault>();

    if constexpr (deducedCallConv.callConv == AS_NAMESPACE_QUALIFIER asCALL_GENERIC) {
        auto callConv = deducedCallConv;
        if (callConv.decl.empty()) {
            constexpr auto funcDecl = GetFuncDecl<F, Opts.AutoHandleDefault, true>();
            callConv.decl = funcDecl;
        }
        AS_NAMESPACE_QUALIFIER asSFuncPtr addr;
        if constexpr (deducedCallConv.genericType == GenericCallConvType::WrapFn) {
            addr = ::gw::id([:F:]).TMPL f<([:F:])>();
        } else if constexpr (deducedCallConv.genericType == GenericCallConvType::WrapMFn) {
            // Use fg variant, i.e. WRAP_MFN_GLOBAL.
            // addr = ::gw::id(&[:F:]).TMPL fg<&([:F:])>();
            // Compiler doesn't like WRAP_MFN_GLOBAL... Not sure why it exists if non-static class methods can't be used
            // with the generic call convention anyway.
            addr = ::gw::id([:F:]).TMPL f<([:F:])>();
        } else if constexpr (deducedCallConv.genericType == GenericCallConvType::WrapObjFirst) {
            // TODO: test as part of object type registration work.
            addr = ::gw::id([:F:]).TMPL of<([:F:])>();
        } else if constexpr (deducedCallConv.genericType == GenericCallConvType::WrapObjLast) {
            // TODO: test as part of object type registration work.
            addr = ::gw::id([:F:]).TMPL ol<([:F:])>();
        } else /* GenericCallConvType::None */ {
            // Even if the function is a class member, it will always be static.
            addr = AS_NAMESPACE_QUALIFIER asFunctionPtr(&[:F:]);
        }
        return Ptr()->RegisterGlobalFunction(callConv.decl.data(), addr, callConvOut = callConv.callConv, auxiliary);
    } else {
        auto callConv = deducedCallConv.callConv;
        if (callConv == AS_NAMESPACE_QUALIFIER asCALL_THISCALL || callConv == AS_NAMESPACE_QUALIFIER asCALL_THISCALL_OBJFIRST
            || callConv == AS_NAMESPACE_QUALIFIER asCALL_THISCALL_OBJLAST) {
            callConv = AS_NAMESPACE_QUALIFIER asCALL_THISCALL_ASGLOBAL;
        }
        AS_NAMESPACE_QUALIFIER asSFuncPtr addr;
        if constexpr (std::meta::is_class_member(F) && !std::meta::is_static_member(F)) {
            addr = AS_NAMESPACE_QUALIFIER asSMethodPtr<sizeof(decltype(&[:F:]))>::Convert(&[:F:]);
        } else {
            addr = AS_NAMESPACE_QUALIFIER asFunctionPtr(&[:F:]);
        }
        return Ptr()->RegisterGlobalFunction(
            GetFuncDecl<F, Opts.AutoHandleDefault, true>().data(), addr, callConvOut = callConv, auxiliary
        );
    }
}

template <EngineOptions Opts> template <std::meta::info F> int Engine<Opts>::RegisterFuncdef() {
    if (!HasEngine()) { return AS_NAMESPACE_QUALIFIER asINVALID_ARG; }
    constexpr auto decl = GetFuncDecl<F, Opts.AutoHandleDefault, true>();
    return Ptr()->RegisterFuncdef(decl.data());
}

template <EngineOptions Opts> template <std::meta::info T> int Engine<Opts>::RegisterTypedef() {
    if (!HasEngine()) { return AS_NAMESPACE_QUALIFIER asINVALID_ARG; }
    constexpr auto typedefName = GetIdentifierOf<T>();
    // Since AngelScript only supports primitive types here, we don't need to worry about SubTypeLists or other complex
    // annotations or types.
    using DealiasedT = typename[:std::meta::dealias(T):];
    constexpr auto typedefDecl = TypeName<DealiasedT>;
    return Ptr()->RegisterTypedef(typedefName.data(), typedefDecl.data());
}

template <EngineOptions Opts> template <std::meta::info E> int Engine<Opts>::RegisterEnum() {
    if (!HasEngine()) { return AS_NAMESPACE_QUALIFIER asINVALID_ARG; }
    constexpr auto enumName = GetIdentifierOf<E>();
    static constexpr auto values = std::define_static_array(std::meta::enumerators_of(E));

    const auto typeId = Ptr()->RegisterEnum(enumName.data());

    template for (constexpr auto v : values) {
        const auto r = Ptr()->RegisterEnumValue(enumName.data(), GetIdentifierOf<v>().data(), static_cast<int>([:v:]));
        if (r < 0) { return r; }
    }

    return typeId;
}

template <EngineOptions Opts> template <std::meta::info I, bool R> int Engine<Opts>::RegisterInterface() {
    if (!HasEngine()) { return AS_NAMESPACE_QUALIFIER asINVALID_ARG; }
    int r = 0;
    static constexpr auto interfaceHierarchy = GetClassHierarchy<I>(R);

    template for (constexpr auto i : interfaceHierarchy) {
        using type = [:i.type:];
        constexpr auto interfaceName = TypeName<type>;
        if (r = Ptr()->RegisterInterface(interfaceName.data()); r < 0) { return r; }

        static constexpr auto members = i.members;
        template for (constexpr auto m : members) {
            if constexpr (std::meta::is_pure_virtual(m) && !std::meta::is_operator_function(m)) {
                constexpr auto decl = GetFuncDecl<m, Opts.AutoHandleDefault>();
                if (r = Ptr()->RegisterInterfaceMethod(interfaceName.data(), decl.data()); r < 0) { return r; }
            }
        }
    }

    return r;
}

/* Prerequisites TODO list:
1. Have ObjFirst or ObjLast default engine setting.
*/

/* Reference types TODO list:
1. Factory function, default and custom.
2. Factory functions with auxiliary objects.
3. List factory functions (see point 3 under value types).
4. AddRef and Release behaviours, including asOBJ_NOCOUNT.
5. Support scoped reference types that only have Release behaviour. ScopedReferenceType.hpp.
    a. https://www.angelcode.com/angelscript/sdk/docs/manual/doc_adv_scoped_type.html
       These types have their own weird @ and &in/&out behaviour... Might need to make this a special type like
       as::RefType.
6. Support asOBJ_NOHANDLE. No factory, addref or release behaviours.
7. Support inheritance. Automatically add opCast and opImplCast methods. Make it work across multiple levels of the
   hierarchy.
8. Support weak refs.
*/

/* Value types TODO list:
1. Tell the difference between a POD type and a non-POD type.
2. Figure out how constructors and destructors will work (similarly to factory functions probably).
3. Figure out how list constructors will work (want to minimize manually writing out the pattern as much as possible).
4. Use asGetTypeTraits().
5. asOBJ_APP_CLASS_MORE_CONSTRUCTORS.
6. asOBJ_APP_CLASS_ALLINTS.
7. asOBJ_APP_CLASS_ALLFLOATS.
8. asOBJ_APP_CLASS_ALIGN8. How can I tell if a class may require 8byte alignment??
9. asOBJ_APP_CLASS_UNION. How would I even register a class that uses unions?
*/

/* Operators TODO list:
1. Create a mapping from display_string_of() to AngelScript op method names.
2. Support both static and non-static operators so long as they are within the class's scope.
3. Do we want to ignore special member functions?
4. Refer to Claude notes below on getting the correct method pointer.
*/

/* Methods TODO list:
1. Ignore non-public, static, and pure virtual methods, along with constructors and destructors.
2. Do we support composite members? Inclination is to ignore this for now, but it might be useful.
3. Detect property accessors, i.e. get_XYZ and set_XYZ methods, and append "property" to the declaration.
*/

/* Properties TODO list:
1. Public properties only.
2. Supporting pointer properties. They require special & syntax, regardless of ref or value type?
3. Ignore composite members.
*/

/* Garbage-collected types TODO list:
1. Implement GarbageCollectedReferenceType.hpp ...
2. ... and GarbageCollectedValueType.hpp abstract classes.
3. Detect presence of garbage collection-related methods/functions and apply asOBJ_GC flag, etc.
*/

/* Template types TODO list:
1. Factory (ref) and constructor (value) hidden parameter: map asITypeInfo* to int&in.
2. Support 1 for list factories/constructors, too.
3. if_handle_then_const.
4. asBEHAVE_TEMPLATE_CALLBACK behaviour.
5. Template specializations.
*/

/* Scoped funcdefs TODO list:
1. Support scoped funcdefs, i.e. if the given function declaration is within a class, scope it to that class:
   https://www.angelcode.com/angelscript/sdk/docs/manual/classas_i_script_engine.html#a03c1a2cc23ae4b742c927f3472a1a4f7.
2. Support scoped funcdefs for template types.
*/

/* Claude on getting member pointers & offsets in relation to e.g. MyDerived:
This is a tricky one because the reflection system gives you members as they were originally declared — an inherited method's
reflection will always refer to the base class it was declared in, so splicing it directly always gives you the base class
pointer.

The key insight is that you don't actually need the address relative to the derived class for methods — what AngelScript
needs is a method pointer that works correctly when called on the derived class, and a base class method pointer already
satisfies that via normal C++ virtual dispatch and implicit pointer conversion. So `&[:BaseClass::MyInheritedMethod:]` cast
to a `BaseClass` method pointer, then registered against `GivenClass`, should work correctly.

However if you do need the derived class pointer explicitly — for example for non-virtual methods where AngelScript needs the
correct `this` pointer adjustment — the approach is to use the type from `GetClassHierarchy` to form the cast explicitly:

```cpp
consteval auto MemberPtrFor(std::meta::info givenClass, std::meta::info member) {
    // If the member is declared in givenClass, splice directly
    if (std::meta::parent_of(member) == givenClass)
        return &[: member :];

    // Otherwise cast the base class pointer to the given class
    // by forming the correct pointer-to-member type
    using GivenType = [: givenClass :];
    using RetType   = [: std::meta::return_type_of(member) :];
    return static_cast<RetType (GivenType::*)(
        [: std::meta::type_of(p) :] ... [: std::meta::parameters_of(member) :]
    )>(&[: member :]);
}
```

The cast is safe because `GivenClass` inherits from `BaseClass`, so a pointer-to-member of `BaseClass` is implicitly
convertible to a pointer-to-member of `GivenClass` — this is the contravariant pointer-to-member conversion rule in C++, and
it's exactly what AngelScript needs to call the method on a `GivenClass` instance.

For **data members** (properties) rather than methods, `offsetof` or `std::meta::offset_of` is the right tool — it gives you
the byte offset of the member relative to any class in the hierarchy, and AngelScript's property registration typically takes
an offset rather than a pointer anyway:

```cpp
consteval std::size_t OffsetOf(std::meta::info givenClass, std::meta::info member) {
    return std::meta::offset_of(member); // offset is absolute, works regardless of which class declared it
}
```

So the short answer is: for methods, use the contravariant pointer-to-member cast; for properties, use `offset_of` which is
already class-agnostic.
*/

// template <std::meta::info T> int Engine::RegisterObjectType() {
//     if (!HasEngine()) { return AS_NAMESPACE_QUALIFIER asINVALID_ARG; }
//     using type = [:T:];
//     const auto typeName = TypeName<type>.c_str();
//     const auto typeIsRef = IsRefType<T>();
//     const auto typeHasFactory = typeIsRef && HasFactoryFunction<type>;
//     const auto typeIsRefCounted = typeIsRef && IsReferenceCounted<type>;
//     const auto typeSize = typeIsRef ? 0 : sizeof(type);
//     auto flags = typeIsRef ? AS_NAMESPACE_QUALIFIER asOBJ_REF : AS_NAMESPACE_QUALIFIER asOBJ_VALUE;
//     // We can apply the asOBJ_NOCOUNT flag automatically if the AddRef and Release behaviours are not implemented.
//     // This approach does force you to implement the behaviours as AddRef and Release member functions.
//     if (typeIsRef && !typeIsRefCounted) { flags |= AS_NAMESPACE_QUALIFIER asOBJ_NOCOUNT; }
//     auto r = Ptr()->RegisterObjectType(typeName, typeSize, flags);
//     if (r < 0) { return r; }

// // Register Factory behaviour.
// if (typeHasFactory) {
//     r = Ptr()->RegisterObjectBehaviour(
//         typeName,
//         AS_NAMESPACE_QUALIFIER asBEHAVE_FACTORY,
//         as::GetFuncDecl<^^type::Factory>(),
//         AS_NAMESPACE_QUALIFIER asFUNCTION(type::Factory),
//         // TODO: replace with updated function.
//         GetFuncCallConv<^^type::Factory>()
//     );
//     if (r < 0) { return r; }
// }

// // Register AddRef and Release behaviours.
// if (typeIsRefCounted) {
//     r = Ptr()->RegisterObjectBehaviour(
//         typeName,
//         AS_NAMESPACE_QUALIFIER asBEHAVE_ADDREF,
//         "void f()",
//         AS_NAMESPACE_QUALIFIER asMETHOD(type, AddRef),
//         AS_NAMESPACE_QUALIFIER asCALL_THISCALL
//     );
//     if (r < 0) { return r; }
//     r = Ptr()->RegisterObjectBehaviour(
//         typeName,
//         AS_NAMESPACE_QUALIFIER asBEHAVE_RELEASE,
//         "void f()",
//         AS_NAMESPACE_QUALIFIER asMETHOD(type, Release),
//         AS_NAMESPACE_QUALIFIER asCALL_THISCALL
//     );
//     if (r < 0) { return r; }
// }
// }
} // namespace as
