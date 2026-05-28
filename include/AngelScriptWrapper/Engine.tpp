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

template <std::meta::info Alias, typename T>
    requires(!IsConst<T>)
int Engine::RegisterGlobalProperty(std::string name, T* const value) {
    // Unfortunately we can't statically validate that the subtype is correct,
    // but this will at least catch incorrect base types.
    static_assert(
        std::meta::dealias(Alias) == ^^T || std::meta::dealias(Alias) == std::meta::dealias(^^const T),
        "The AngelScript Specialization's base type you gave does not match the type of the value given!"
    );
    if (!HasEngine()) { return AS_NAMESPACE_QUALIFIER asINVALID_ARG; }
    name = GetTypeDecl<Alias>() + " " + name;
    return Ptr()->RegisterGlobalProperty(name.c_str(), value);
}
} // namespace as
