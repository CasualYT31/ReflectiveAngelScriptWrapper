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
    if (!HasEngine()) { return AS_NAMESPACE_QUALIFIER asINVALID_ARG; }
    name = GetTypeDecl<Alias>() + " " + name;
    return Ptr()->RegisterGlobalProperty(name.c_str(), value);
}
} // namespace as
