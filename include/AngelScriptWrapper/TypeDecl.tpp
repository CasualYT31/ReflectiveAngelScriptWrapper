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
template <> inline constexpr std::string_view TypeName<void> = "void";

template <> inline constexpr std::string_view TypeName<bool> = "bool";

template <> inline constexpr std::string_view TypeName<std::int8_t> = "int8";

template <> inline constexpr std::string_view TypeName<std::uint8_t> = "uint8";

template <> inline constexpr std::string_view TypeName<std::int16_t> = "int16";

template <> inline constexpr std::string_view TypeName<std::uint16_t> = "uint16";

template <> inline constexpr std::string_view TypeName<std::int32_t> = "int32";

template <> inline constexpr std::string_view TypeName<std::uint32_t> = "uint32";

template <> inline constexpr std::string_view TypeName<std::int64_t> = "int64";

template <> inline constexpr std::string_view TypeName<std::uint64_t> = "uint64";

template <> inline constexpr std::string_view TypeName<float> = "float";

template <> inline constexpr std::string_view TypeName<double> = "double";

template <> inline constexpr std::string_view TypeName<std::string> = "string";

template <> inline constexpr std::string_view TypeName<AS_NAMESPACE_QUALIFIER CDateTime> = "datetime";

template <> inline constexpr std::string_view TypeName<AS_NAMESPACE_QUALIFIER Complex> = "complex";

template <> inline constexpr std::string_view TypeName<AS_NAMESPACE_QUALIFIER CScriptAny> = "any";

template <> inline constexpr std::string_view TypeName<AS_NAMESPACE_QUALIFIER CScriptArray> = "array";

template <> inline constexpr std::string_view TypeName<AS_NAMESPACE_QUALIFIER CScriptDictionary> = "dictionary";

template <> inline constexpr std::string_view TypeName<AS_NAMESPACE_QUALIFIER CScriptDictValue> = "dictionaryValue";

template <> inline constexpr std::string_view TypeName<AS_NAMESPACE_QUALIFIER CScriptFile> = "file";

template <> inline constexpr std::string_view TypeName<AS_NAMESPACE_QUALIFIER CScriptFileSystem> = "filesystem";

template <> inline constexpr std::string_view TypeName<AS_NAMESPACE_QUALIFIER CScriptGrid> = "grid";

template <> inline constexpr std::string_view TypeName<AS_NAMESPACE_QUALIFIER CScriptHandle> = "ref";

template <> inline constexpr std::string_view TypeName<AS_NAMESPACE_QUALIFIER CScriptSocket> = "socket";

// Note that const_weakref is not supported.
// Can't you just do weakref<const T> anyway?
template <> inline constexpr std::string_view TypeName<AS_NAMESPACE_QUALIFIER CScriptWeakRef> = "weakref";

// It's important to handle these specializations in this order:

template <typename T>
constexpr std::string_view TypeName<const T> =
    std::define_static_string("const " + std::string(TypeName<std::remove_const_t<T>>));

template <typename T>
constexpr std::string_view TypeName<T*> =
    std::define_static_string(std::string(TypeName<std::remove_pointer_t<T>>) + std::string(GetRefType<T>()));

template <typename T>
constexpr std::string_view TypeName<T* const> = std::define_static_string(
    std::string(TypeName<std::remove_pointer_t<std::remove_const_t<T>>>) + std::string(GetRefType<T, true>())
);

template <typename T>
constexpr std::string_view TypeName<const T* const> = std::define_static_string(
    "const " + std::string(TypeName<std::remove_pointer_t<std::remove_const_t<T>>>) + std::string(GetRefType<T, true>())
);

namespace detail {
template <typename T, SubTypes S> consteval std::string_view GetTypeDecl() {
    // First, construct the subtype list.
    std::string subTypes;
    template for (constexpr std::size_t i : std::views::iota(0uz, S.subTypes.size())) {
        if constexpr (S.recursiveSub.empty()) {
            subTypes += std::string(TypeName<typename[:S.subTypes[i]:]>);
        } else {
            subTypes += std::string(GetTypeDecl<typename[:S.subTypes[i]:], S.recursiveSub[i]>());
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
            decl = "const " + std::string(TypeName<std::remove_cvref_t<std::remove_pointer_t<T>>>) + subTypes
                   + std::string(GetRefType<T, true>());
        } else {
            decl = std::string(TypeName<std::remove_cvref_t<std::remove_pointer_t<T>>>) + subTypes
                   + std::string(GetRefType<T, true>());
        }
    } else if constexpr (IsPointer<T>) {
        // "@/&" suffix.
        if constexpr (IsConst<std::remove_pointer_t<std::remove_const_t<T>>>) {
            // "const" prefix.
            decl = "const " + std::string(TypeName<std::remove_cvref_t<std::remove_pointer_t<T>>>) + subTypes
                   + std::string(GetRefType<T>());
        } else {
            decl = std::string(TypeName<std::remove_cvref_t<std::remove_pointer_t<T>>>) + subTypes
                   + std::string(GetRefType<T>());
        }
    } else if constexpr (IsConst<T>) {
        // "const" prefix.
        decl = "const " + std::string(TypeName<std::remove_cvref_t<std::remove_pointer_t<T>>>) + subTypes;
    } else {
        decl = std::string(TypeName<std::remove_cvref_t<std::remove_pointer_t<T>>>) + subTypes;
    }
    return std::define_static_string(decl);
}

template <std::meta::info I> consteval SubTypes GetSubTypes() {
    constexpr auto subTypesAnnotation = std::define_static_array(std::meta::annotations_of_with_type(I, ^^SubTypes));
    static_assert(
        subTypesAnnotation.size() <= 1,
        std::string(std::meta::display_string_of(I)) + " was given more than one SubTypes annotation"
    );
    // TODO: Will likely introduce a Template annotation for types soon? We can then static assert that SubTypes are
    //       only given to AngelScript template types.
    return subTypesAnnotation.size() == 0 ? SubTypes{} : std::meta::extract<SubTypes>(subTypesAnnotation[0]);
}

template <std::meta::info I> consteval std::string_view GetTypeDecl() {
    // First, extract the subtypes assigned to the template type of the given variable/function/parameter/etc.
    constexpr SubTypes subTypes = GetSubTypes<I>();

    // Next, figure out which type we should work with.
    // If I reflects a function, use its return type.
    // Otherwise use type_of().
    constexpr auto baseType = std::meta::is_function(I) ? std::meta::return_type_of(I) : std::meta::type_of(I);

    // Finally, defer the rest of the type declaration generation to the other template function.
    return GetTypeDecl<typename[:baseType:], subTypes>();
}

template <typename T, std::meta::info I> consteval std::string_view GetTypeDecl() {
    return GetTypeDecl<T, GetSubTypes<I>()>();
}
}; // namespace detail

namespace detail {
consteval SubTypes SubTypeList(std::vector<std::meta::info> types) {
    std::vector<std::meta::info> subTypes;
    std::vector<SubTypes> recursiveSub;
    for (auto T : types) {
        if (std::meta::has_template_arguments(T) && std::meta::template_of(T) == ^^Tmpl) {
            auto args = std::meta::template_arguments_of(T);
            subTypes.push_back(args[0]);
            args.erase(args.begin());
            recursiveSub.push_back(SubTypeList(args));
        } else {
            subTypes.push_back(T);
            recursiveSub.emplace_back();
        }
    }
    return SubTypes{
        .subTypes = std::define_static_array(subTypes),
        .recursiveSub = std::define_static_array(recursiveSub),
    };
}
} // namespace detail

template <typename... Ts> consteval SubTypes SubTypeList() {
    return detail::SubTypeList({ ^^Ts... });
}

template <typename T> constexpr bool IsRefType() {
    return std::meta::annotations_of_with_type(
               std::meta::dealias(^^std::remove_cvref_t<remove_all_pointers_t<T>>), ^^decltype(RefType)
           )
               .size()
           > 0;
}

template <typename T, bool C> constexpr std::string_view GetRefType() {
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
} // namespace as
