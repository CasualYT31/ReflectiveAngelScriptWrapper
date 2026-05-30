/**
 * @file TypeDecl.tpp
 * The implementations of template functions found within TypeDecl.
 */

#pragma once

BEGIN_AS_NAMESPACE

class CDateTime;
class Complex;
class[[= as::RefType]] CScriptAny;
class[[= as::RefType]] CScriptArray;
class[[= as::RefType]] CScriptDictionary;
class CScriptDictValue;
class[[= as::RefType]] CScriptFile;
class[[= as::RefType]] CScriptFileSystem;
class[[= as::RefType]] CScriptGrid;
class CScriptHandle;
class[[= as::RefType]] CScriptSocket;
class CScriptWeakRef;

END_AS_NAMESPACE

namespace as {
template <typename T> constexpr std::string GetTypeDecl() {
    return std::string(std::meta::identifier_of(^^T));
}

template <> constexpr std::string GetTypeDecl<void>() {
    return "void";
}

template <> constexpr std::string GetTypeDecl<bool>() {
    return "bool";
}

template <> constexpr std::string GetTypeDecl<std::int8_t>() {
    return "int8";
}

template <> constexpr std::string GetTypeDecl<std::uint8_t>() {
    return "uint8";
}

template <> constexpr std::string GetTypeDecl<std::int16_t>() {
    return "int16";
}

template <> constexpr std::string GetTypeDecl<std::uint16_t>() {
    return "uint16";
}

template <> constexpr std::string GetTypeDecl<std::int32_t>() {
    return "int32";
}

template <> constexpr std::string GetTypeDecl<std::uint32_t>() {
    return "uint32";
}

template <> constexpr std::string GetTypeDecl<std::int64_t>() {
    return "int64";
}

template <> constexpr std::string GetTypeDecl<std::uint64_t>() {
    return "uint64";
}

template <> constexpr std::string GetTypeDecl<float>() {
    return "float";
}

template <> constexpr std::string GetTypeDecl<double>() {
    return "double";
}

template <> constexpr std::string GetTypeDecl<std::string>() {
    return "string";
}

template <> constexpr std::string GetTypeDecl<AS_NAMESPACE_QUALIFIER CDateTime>() {
    return "datetime";
}

template <> constexpr std::string GetTypeDecl<AS_NAMESPACE_QUALIFIER Complex>() {
    return "complex";
}

template <> constexpr std::string GetTypeDecl<AS_NAMESPACE_QUALIFIER CScriptAny>() {
    return "any";
}

template <> constexpr std::string GetTypeDecl<AS_NAMESPACE_QUALIFIER CScriptArray>() {
    return "array";
}

template <> constexpr std::string GetTypeDecl<AS_NAMESPACE_QUALIFIER CScriptDictionary>() {
    return "dictionary";
}

template <> constexpr std::string GetTypeDecl<AS_NAMESPACE_QUALIFIER CScriptDictValue>() {
    return "dictionaryValue";
}

template <> constexpr std::string GetTypeDecl<AS_NAMESPACE_QUALIFIER CScriptFile>() {
    return "file";
}

template <> constexpr std::string GetTypeDecl<AS_NAMESPACE_QUALIFIER CScriptFileSystem>() {
    return "filesystem";
}

template <> constexpr std::string GetTypeDecl<AS_NAMESPACE_QUALIFIER CScriptGrid>() {
    return "grid";
}

template <> constexpr std::string GetTypeDecl<AS_NAMESPACE_QUALIFIER CScriptHandle>() {
    return "ref";
}

template <> constexpr std::string GetTypeDecl<AS_NAMESPACE_QUALIFIER CScriptSocket>() {
    return "socket";
}

// Note that const_weakref is not supported.
// Can't you just do weakref<const T> anyway?
template <> constexpr std::string GetTypeDecl<AS_NAMESPACE_QUALIFIER CScriptWeakRef>() {
    return "weakref";
}

// It's important to handle these specializations in this order:

template <typename T>
    requires IsConst<T>
constexpr std::string GetTypeDecl() {
    return "const " + GetTypeDecl<std::remove_const_t<T>>();
}

template <typename T>
    requires IsPointer<T>
constexpr std::string GetTypeDecl() {
    return GetTypeDecl<std::remove_pointer_t<T>>() + GetRefType<T>();
}

template <typename T>
    requires IsConst<T> && IsPointer<T>
constexpr std::string GetTypeDecl() {
    return GetTypeDecl<std::remove_pointer_t<std::remove_const_t<T>>>() + GetRefType<T, true>();
}

template <std::meta::info Alias> constexpr std::string GetTypeDecl() {
    if constexpr (std::meta::has_parent(Alias)) {
        constexpr auto TParent = std::meta::parent_of(Alias);
        if constexpr (std::meta::is_type(TParent)) {
            using ParentType = typename[:TParent:];
            if constexpr (HasCustomTypeDecl<ParentType>) {
                return ParentType::GetTypeDecl();
            } else {
                static_assert(
                    false,
                    "you provided an invalid type alias to GetTypeDecl(), make sure you provide the ^^Specialize<B, T>::T "
                    "type alias"
                );
                return "";
            }
        } else {
            static_assert(
                false,
                "you provided an invalid type alias to GetTypeDecl(), make sure you provide the ^^Specialize<B, T>::T type "
                "alias"
            );
            return "";
        }
    } else {
        static_assert(
            false,
            "you provided an invalid type alias to GetTypeDecl(), make sure you provide the ^^Specialize<B, T>::T type alias"
        );
        return "";
    }
}

template <std::meta::info T> constexpr bool IsRefType() {
    return std::meta::annotations_of_with_type(std::meta::dealias(T), ^^decltype(RefType)).size() > 0;
}

template <std::meta::info F> constexpr AS_NAMESPACE_QUALIFIER asDWORD GetFuncCallConv() {
    constexpr auto callConv = std::define_static_array(std::meta::annotations_of_with_type(F, ^^CallConv));
    static_assert(
        callConv.size() <= 1,
        "function " + std::string(std::meta::display_string_of(F)) + " was given more than one CallConv annotation"
    );
    if constexpr (callConv.size() == 1) {
        return std::meta::extract<CallConv>(callConv.at(0)).is;
    } else if constexpr (std::meta::has_parent(F) && std::meta::is_type(std::meta::parent_of(F))
                         && !std::meta::is_static_member(F)) {
        return AS_NAMESPACE_QUALIFIER asCALL_THISCALL;
    } else {
        return AS_NAMESPACE_QUALIFIER asCALL_CDECL;
    }
}

template <typename T, bool C> constexpr std::string GetRefType() {
    if constexpr (IsRefType<^^remove_all_pointers_t<T>>()) {
        if constexpr (C) {
            return "@ const";
        } else {
            return "@";
        }
    } else {
        return "&";
    }
}
} // namespace as
