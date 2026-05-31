/**
 * @file FuncDecl.tpp
 * The implementations of template functions found within TypeDecl.
 */

#pragma once

#include <AngelScriptWrapper/TypeDecl.hpp>

namespace as {
namespace detail {
template <std::meta::info P> constexpr std::string GetFuncTypeDecl() {
    /* Ref types:
    obj                                 obj (Copy?)
    obj& / obj@                         obj*
    const obj& / const obj@             const obj*
    const obj& / const obj@ const       const obj* const

    Value types:
    val             val
    val&out         val& / val*
    const val&in    val const&
    */
    using T = [:std::meta::type_of(P):];
    if constexpr (IsRefType<T>()) {
        if constexpr (IsPointer<T>) {
            if constexpr (AsHandle<P>) {
                // TODO: handle + modifier for @. Can't just append it as
                //       "...@ const" could be returned (but maybe this is fine)?
                return GetTypeDecl<T, P>();
            } else {
                return GetTypeDecl<std::remove_pointer_t<T>, P>() + "&";
            }
        } else {
            return GetTypeDecl<T, P>();
        }
    } else {
        if constexpr (IsReference<T>) {
            using Tr = std::remove_reference_t<T>;
            static_assert(!IsPointer<Tr>, "Detected parameter*&, which is not currently supported[?]");
            if constexpr (IsConst<Tr>) {
                return GetTypeDecl<Tr, P>() + "&in";
            } else {
                return GetTypeDecl<Tr, P>() + "&out";
            }
        } else if constexpr (IsPointer<T>) {
            using Tp = std::remove_pointer_t<T>;
            if constexpr (IsConst<Tp>) {
                return GetTypeDecl<Tp, P>() + "&in";
            } else {
                return GetTypeDecl<Tp, P>() + "&out";
            }
        } else {
            if constexpr (std::meta::is_const(P)) {
                return "const " + GetTypeDecl<T, P>();
            } else {
                return GetTypeDecl<T, P>();
            }
        }
    }
}

template <std::meta::info P> constexpr std::string DefaultValueOrEmpty() {
    constexpr auto defVal = std::define_static_array(std::meta::annotations_of_with_type(P, ^^DefaultsTo));
    static_assert(
        defVal.size() <= 1,
        std::string(std::meta::display_string_of(P)) + " was given more than one DefaultsTo/DefVal annotation"
    );
    if constexpr (defVal.size() == 1) {
        return " = " + std::string(std::meta::extract<DefaultsTo>(defVal.at(0)).value);
    } else {
        return "";
    }
}
} // namespace detail

template <std::meta::info P> constexpr bool AsHandle() {
    return std::meta::annotations_of_with_type(P, ^^decltype(Handle)).size() > 0;
}

template <std::meta::info F> constexpr std::string GetFuncDecl() {
    // Return type.
    std::string decl = GetTypeDecl<typename[:std::meta::return_type_of(F):], F>();
    // Name.
    decl += " " + std::string(std::meta::identifier_of(F)) + "(";
    // Parameter list.
    template for (constexpr auto P : std::define_static_array(std::meta::parameters_of(F))) {
        decl += detail::GetFuncTypeDecl<P>() + detail::DefaultValueOrEmpty<P>() + ", ";
    }
    // Remove last comma.
    if constexpr (std::meta::parameters_of(F).size() > 0) { decl.erase(decl.size() - 2); }
    // Close.
    decl += ")";
    // Is this a const method?
    if constexpr (std::meta::is_const(F)) { decl += " const"; }
    return decl;
}
} // namespace as
