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

template <typename T, TmplSubTypes S> consteval std::string GetTypeDecl() {
    // First, construct the subtype list.
    std::string subTypes;
    template for (constexpr std::size_t i : std::views::iota(0uz, S.subTypes.size())) {
        if constexpr (S.recursiveSub.empty()) {
            subTypes += GetTypeDecl<typename[:S.subTypes[i]:]>();
        } else {
            subTypes += GetTypeDecl<typename[:S.subTypes[i]:], S.recursiveSub[i]>();
        }
        if constexpr (i + 1 < S.subTypes.size()) { subTypes += ", "; }
    }
    if (subTypes.size() > 0) { subTypes = "<" + subTypes + ">"; }

    // Then, construct the base template type declaration with the correct qualifiers.
    std::string decl;
    if constexpr (IsPointer<T> && IsConst<T>) {
        // "@ const/&" suffix.
        if constexpr (IsConst<std::remove_pointer_t<std::remove_const_t<T>>>) {
            // "const" prefix.
            decl = "const " + as::GetTypeDecl<std::remove_cvref_t<std::remove_pointer_t<T>>>() + subTypes
                   + as::GetRefType<T, true>();
        } else {
            decl = as::GetTypeDecl<std::remove_cvref_t<std::remove_pointer_t<T>>>() + subTypes + as::GetRefType<T, true>();
        }
    } else if constexpr (IsPointer<T>) {
        // "@/&" suffix.
        if constexpr (IsConst<std::remove_pointer_t<std::remove_const_t<T>>>) {
            // "const" prefix.
            decl =
                "const " + as::GetTypeDecl<std::remove_cvref_t<std::remove_pointer_t<T>>>() + subTypes + as::GetRefType<T>();
        } else {
            decl = as::GetTypeDecl<std::remove_cvref_t<std::remove_pointer_t<T>>>() + subTypes + as::GetRefType<T>();
        }
    } else if constexpr (IsConst<T>) {
        // "const" prefix.
        decl = "const " + as::GetTypeDecl<std::remove_cvref_t<std::remove_pointer_t<T>>>() + subTypes;
    } else {
        decl = as::GetTypeDecl<std::remove_cvref_t<std::remove_pointer_t<T>>>() + subTypes;
    }
    return decl;
}

template <typename T, std::meta::info S> constexpr std::string GetTypeDecl() {
    constexpr auto subTypes = GetSubTypes<S>();
    if constexpr (subTypes.recursiveSub.empty()) {
        return GetTypeDecl<T>();
    } else {
        return GetTypeDecl<T, subTypes>();
    }
}

template <std::meta::info S> constexpr std::string GetTypeDecl() {
    return GetTypeDecl<typename[:std::meta::type_of(S):], S>();
}

namespace detail {
consteval TmplSubTypes SubType(std::vector<std::meta::info> types) {
    std::vector<std::meta::info> subTypes;
    std::vector<TmplSubTypes> recursiveSub;
    for (auto T : types) {
        if (std::meta::has_template_arguments(T) && std::meta::template_of(T) == ^^Tmpl) {
            auto args = std::meta::template_arguments_of(T);
            subTypes.push_back(args[0]);
            args.erase(args.begin());
            recursiveSub.push_back(SubType(args));
        } else {
            subTypes.push_back(T);
            recursiveSub.emplace_back();
        }
    }
    return TmplSubTypes{
        .subTypes = std::define_static_array(subTypes),
        .recursiveSub = std::define_static_array(recursiveSub),
    };
}
} // namespace detail

template <typename... Ts> consteval TmplSubTypes SubType() {
    return detail::SubType({ ^^Ts... });
}

template <typename... Ts> consteval TmplSubTypes SubTypes() {
    return SubType<Ts...>();
}

template <std::meta::info I> consteval TmplSubTypes GetSubTypes() {
    constexpr auto subTypes = std::define_static_array(std::meta::annotations_of_with_type(I, ^^TmplSubTypes));
    static_assert(
        subTypes.size() <= 1,
        std::string(std::meta::display_string_of(I)) + " was given more than one TmplSubTypes/SubType[s] annotation"
    );
    if constexpr (subTypes.size() == 0) { return {}; }
    return std::meta::extract<TmplSubTypes>(subTypes[0]);
}

template <typename T> constexpr bool IsRefType() {
    return std::meta::annotations_of_with_type(
               std::meta::dealias(^^std::remove_cvref_t<remove_all_pointers_t<T>>), ^^decltype(RefType)
           )
               .size()
           > 0;
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
    if constexpr (IsRefType<T>()) {
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
