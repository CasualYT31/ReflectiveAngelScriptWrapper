/**
 * @file Specialize.tpp
 * The implementation of the Specialize template struct.
 */

#pragma once

namespace as {
template <typename B, typename S> constexpr std::string Specialize<B, S>::GetTypeDecl() {
    if constexpr (IsPointer<B> && IsConst<B>) {
        // "@ const" suffix.
        if constexpr (IsConst<std::remove_pointer_t<std::remove_const_t<B>>>) {
            // "const" prefix.
            return "const " + as::GetTypeDecl<std::remove_cvref_t<std::remove_pointer_t<B>>>() + "<" + as::GetTypeDecl<S>()
                   + ">@ const";
        } else {
            return as::GetTypeDecl<std::remove_cvref_t<std::remove_pointer_t<B>>>() + "<" + as::GetTypeDecl<S>()
                   + ">@ const";
        }
    } else if constexpr (IsPointer<B>) {
        // "@" suffix.
        if constexpr (IsConst<std::remove_pointer_t<std::remove_const_t<B>>>) {
            // "const" prefix.
            return "const " + as::GetTypeDecl<std::remove_cvref_t<std::remove_pointer_t<B>>>() + "<" + as::GetTypeDecl<S>()
                   + ">@";
        } else {
            return as::GetTypeDecl<std::remove_cvref_t<std::remove_pointer_t<B>>>() + "<" + as::GetTypeDecl<S>() + ">@";
        }
    } else if constexpr (IsConst<B>) {
        // "const" prefix.
        return "const " + as::GetTypeDecl<std::remove_cvref_t<std::remove_pointer_t<B>>>() + "<" + as::GetTypeDecl<S>()
               + ">";
    } else {
        return as::GetTypeDecl<std::remove_cvref_t<std::remove_pointer_t<B>>>() + "<" + as::GetTypeDecl<S>() + ">";
    }
}
} // namespace as
