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
        constexpr auto interfaceName = GetIdentifierOf<i.type>();
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
