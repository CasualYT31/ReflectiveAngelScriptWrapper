#pragma once

#include <algorithm>

BEGIN_AS_NAMESPACE

class CDateTime;
class Complex;
class CScriptAny;
class CScriptArray;
class CScriptDictionary;
class CScriptDictValue;
class CScriptFile;
class CScriptFileSystem;
class CScriptGrid;
class CScriptHandle;
class CScriptSocket;
class CScriptWeakRef;

END_AS_NAMESPACE

namespace as {
template <std::meta::info O> consteval std::string_view GetIdentifierOf() {
    constexpr auto rename = ExtractAnnotation<O, Rename>();
    if constexpr (rename) {
        return std::string_view(rename->to);
    } else {
        return std::meta::identifier_of(O);
    }
}

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
    // TODO: Will likely introduce a Template annotation for types soon? We can then static assert that SubTypes are
    //       only given to AngelScript template types.
    constexpr auto subTypesAnnotation = ExtractAnnotation<I, SubTypes>();
    return subTypesAnnotation ? *subTypesAnnotation : SubTypes{};
}

template <std::meta::info T> constexpr bool IsFuncdefHandle() {
    return T == ^^AS_NAMESPACE_QUALIFIER asIScriptFunction* || T == ^^AS_NAMESPACE_QUALIFIER asIScriptFunction* const;
}

template <std::meta::info T, std::meta::info I> consteval std::string_view GetFuncdef() {
    // TODO: support scoped funcdefs, i.e. if the given function declaration is within a class, scope it to that class:
    //       https://www.angelcode.com/angelscript/sdk/docs/manual/classas_i_script_engine.html#a03c1a2cc23ae4b742c927f3472a1a4f7.
    constexpr auto funcdef = ExtractAnnotation<I, Funcdef>();
    static_assert(funcdef, "asIScriptFunction* object or parameter was not given a Funcdef annotation");
    std::string r = std::string(std::meta::identifier_of(funcdef->funcdef)) + "@";
    if constexpr (std::meta::is_const(T)) { r += " const"; }
    return std::define_static_string(r);
}

template <std::meta::info I> consteval std::string_view GetTypeDecl() {
    // Figure out which type we should work with.
    // If I reflects a function, use its return type.
    // Otherwise use type_of().
    constexpr auto baseType = std::meta::is_function(I) ? std::meta::return_type_of(I) : std::meta::type_of(I);

    if constexpr (IsFuncdefHandle<baseType>()) {
        // If the base type is asIScriptFunction, then use its attached funcdef name.
        return GetFuncdef<baseType, I>();
    } else {
        // Extract the subtypes assigned to the template type of the given variable/function/parameter/etc.
        constexpr SubTypes subTypes = GetSubTypes<I>();

        // Finally, defer the rest of the type declaration generation to the other template function.
        return GetTypeDecl<typename[:baseType:], subTypes>();
    }
}

template <typename T, std::meta::info I> consteval std::string_view GetTypeDecl() {
    if constexpr (IsFuncdefHandle<^^T>()) {
        return GetFuncdef<^^T, I>();
    } else {
        return GetTypeDecl<T, GetSubTypes<I>()>();
    }
}

struct ClassMembers {
    std::meta::info type;

    std::vector<std::meta::info> bases;

    std::vector<std::meta::info> members;

    std::vector<std::string> memberIdentifiers;
};

template <std::meta::info M> consteval std::string GetUniqueIdentifierOfMember() {
    if constexpr (std::meta::has_identifier(M)) {
        return std::string(std::meta::identifier_of(M));
    } else if constexpr (std::meta::is_operator_function(M)) {
        auto ds = std::string(std::meta::display_string_of(M));
        ds = ds.substr(0, ds.find('('));
        ds = ds.substr(ds.find("operator"));
        // operator() will appear as just operator, this is fine for this function's purposes.
        return ds;
    } else {
        return "";
    }
}

template <std::meta::info M> consteval std::string GetUniqueSignatureOfMember() {
    std::string ret = "";
    ret += GetUniqueIdentifierOfMember<M>();
    if (std::meta::is_function(M)) {
        const auto params = std::meta::parameters_of(M);
        for (std::size_t i = 0; i < params.size(); ++i) {
            ret += std::string(std::meta::display_string_of(std::meta::type_of(params[i])));
        }
        if (std::meta::is_const(M)) { ret += "const"; }
    }
    return ret;
}

template <std::meta::info C>
consteval bool FindMembersOf(detail::ClassMembers& members, std::vector<std::meta::info>& visited) {
    for (auto v : visited) {
        if (v == C) { return false; }
    }
    visited.push_back(C);

    template for (constexpr auto m :
                  std::define_static_array(std::meta::members_of(C, std::meta::access_context::current()))) {
        if constexpr (!std::meta::is_special_member_function(m) && !std::meta::is_constructor(m)
                      && !std::meta::is_destructor(m) && !std::meta::is_type(m)) {
            const auto id = GetUniqueSignatureOfMember<m>();
            if (!id.empty() && !std::ranges::contains(members.memberIdentifiers, id)) {
                members.members.push_back(m);
                members.memberIdentifiers.push_back(id);
            }
        }
    }

    template for (constexpr auto b :
                  std::define_static_array(std::meta::bases_of(C, std::meta::access_context::current()))) {
        constexpr auto bType = std::meta::type_of(b);
        if (FindMembersOf<bType>(members, visited)) { members.bases.push_back(bType); }
    }

    return true;
}

template <std::meta::info C>
consteval void FindMembersRecursiveOf(
    std::vector<detail::ClassMembers>& classes, std::vector<std::meta::info>& visited, const bool recurse
) {
    for (auto v : visited) {
        if (v == C) { return; }
    }
    visited.push_back(C);

    classes.emplace_back(C);
    std::vector<std::meta::info> visitedInner;
    detail::FindMembersOf<C>(classes.back(), visitedInner);

    if (recurse) {
        template for (constexpr auto b :
                      std::define_static_array(std::meta::bases_of(C, std::meta::access_context::current()))) {
            constexpr auto bType = std::meta::type_of(b);
            FindMembersRecursiveOf<bType>(classes, visited, recurse);
        }
    }
}
}; // namespace detail

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

template <std::meta::info C> consteval StructuralSpan<const ClassMembers> GetClassHierarchy(const bool recurse) {
    std::vector<detail::ClassMembers> ret;
    std::vector<std::meta::info> visited;
    detail::FindMembersRecursiveOf<C>(ret, visited, recurse);

    std::vector<ClassMembers> staticRet;
    for (const auto r : ret) {
        staticRet.emplace_back(r.type, std::define_static_array(r.bases), std::define_static_array(r.members));
    }
    return std::define_static_array(staticRet);
}
} // namespace as
