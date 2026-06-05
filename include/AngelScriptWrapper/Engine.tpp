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

/*
First, we need a way for the developer to either set CDECL or STDCALL as the default call convention.
We could define this via a template parameter on Engine but that feels wrong.
I think setting this at runtime will suffice.

Then, we introduce CDECL and STDCALL annotations on functions. If these are given explicitly,
then Engine will usually endeavour to use them, no matter what the default is.

We can always detect the generic call convention if return is void and the only parameter is asIScriptGeneric*.
If a function has this convention (besides non-static class members, due to this pointer), then ALWAYS use generic,
even if specified otherwise via annotations.

THISCALL is simple: non-static class member? Yes, and ignore any CDECL/STDCALL annotation. Otherwise no.
THISCALL_ASGLOBAL when RegisterGlobalFunction is being used.

Only difficult situation is the _OBJFIRST and _OBJLAST variants. There is no way for us to reliably tell
what the developer intends, even with reflection. E.g. what if a function both starts and ends with an object
of the same type, how does it know which is intended to be the _OBJ? For these cases we will need to resort to
annotations again: as::ObjFirst and as::ObjLast. They are call convention agnostic and will be applied if
the call convention is decided to be CDECL or THISCALL. They will be ignored if STDCALL is decided, though
it is worth noting that support for it is possible, just not added.
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
