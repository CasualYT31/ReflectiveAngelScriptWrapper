/**
 * @file Engine.tpp
 * The implementations of template methods found within Engine.
 */

#pragma once

#include <AngelScriptWrapper/TypeDecl.hpp>

namespace as {
template <typename T>
    requires(!IsConst<T>)
int Engine::RegisterGlobalProperty(std::string name, T* const value, const bool constant) {
    if (!HasEngine()) { return AS_NAMESPACE_QUALIFIER asINVALID_ARG; }
    if (constant) {
        name = GetTypeDecl<const T>() + " " + name;
    } else {
        name = GetTypeDecl<T>() + " " + name;
    }
    return Ptr()->RegisterGlobalProperty(name.c_str(), value);
}

template <std::meta::info V, typename T>
    requires(!IsConst<T>)
int Engine::RegisterGlobalProperty(std::string name, T* const value, const bool constant) {
    if (!HasEngine()) { return AS_NAMESPACE_QUALIFIER asINVALID_ARG; }
    if (constant) {
        constexpr auto constType = std::define_static_string(GetTypeDecl<const T, V>());
        name = std::string(constType) + " " + name;
    } else {
        constexpr auto type = std::define_static_string(GetTypeDecl<T, V>());
        name = std::string(type) + " " + name;
    }
    return Ptr()->RegisterGlobalProperty(name.c_str(), value);
}

template <std::meta::info V, typename T>
    requires(!IsConst<T>)
int Engine::RegisterGlobalProperty(std::string name, OwnedObject<T> const& value, const bool constant) {
    if (!HasEngine()) { return AS_NAMESPACE_QUALIFIER asINVALID_ARG; }
    if (constant) {
        constexpr auto constType = std::define_static_string(GetTypeDecl<const T, V>());
        name = std::string(constType) + " " + name;
    } else {
        constexpr auto type = std::define_static_string(GetTypeDecl<T, V>());
        name = std::string(type) + " " + name;
    }
    return Ptr()->RegisterGlobalProperty(name.c_str(), value.Ptr());
}

// template <std::meta::info F> int Engine::RegisterGlobalFunction(void* auxiliary) {
//     // if (!HasEngine()) { return AS_NAMESPACE_QUALIFIER asINVALID_ARG; }
//     // /* callConv
//     // asCALL_CDECL              Can't tell from stdcall?
//     // asCALL_STDCALL            Can't tell from cdecl?
//     // asCALL_GENERIC            asIScriptGeneric* only parameter.
//     // asCALL_CDECL_OBJFIRST     Assuming this passes aux as first param?
//     // asCALL_CDECL_OBJLAST      Same but last param?
//     // asCALL_THISCALL_ASGLOBAL  Function is a non-static member but aux is given.
//     // asCALL_THISCALL_OBJFIRST  How does this work for global funcs?
//     // asCALL_THISCALL_OBJLAST   Ditto?
//     // */
//     // return Ptr()->RegisterGlobalFunction(GetFuncDecl<F>()
//     //                                      // funcPtr,
//     //                                      // callConv,
//     //                                      // auxiliary
//     // );
//     return 0;
// }

// template <std::meta::info T> int Engine::RegisterObjectType() {
//     if (!HasEngine()) { return AS_NAMESPACE_QUALIFIER asINVALID_ARG; }
//     using type = [:T:];
//     const auto typeName = GetTypeDecl<type>().c_str();
//     const auto typeIsRef = IsRefType<T>();
//     const auto typeHasFactory = typeIsRef && HasFactoryFunction<type>;
//     const auto typeIsRefCounted = typeIsRef && IsReferenceCounted<type>;
//     const auto typeSize = typeIsRef ? 0 : sizeof(type);
//     auto flags = typeIsRef ? AS_NAMESPACE_QUALIFIER asOBJ_REF : AS_NAMESPACE_QUALIFIER asOBJ_VALUE;
//     // We can apply the asOBJ_NOCOUNT flag automatically if the AddRef and Release behaviours are not implemented.
//     // This approach does force you to implement the behaviours as AddRef and Release member functions.
//     if (!typeIsRefCounted) { flags |= AS_NAMESPACE_QUALIFIER asOBJ_NOCOUNT; }
//     auto r = Ptr()->RegisterObjectType(typeName, typeSize, flags);
//     if (r < 0) { return r; }

// // Register Factory behaviour.
// if (typeHasFactory) {
//     r = Ptr()->RegisterObjectBehaviour(
//         typeName,
//         AS_NAMESPACE_QUALIFIER asBEHAVE_FACTORY,
//         "",
//         AS_NAMESPACE_QUALIFIER asFUNCTION(type::Factory),
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
