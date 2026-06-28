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
    static_assert(!std::meta::has_template_arguments(O), "template types require explicit specialization of TypeName");
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

template <> inline constexpr std::string_view TypeName<AS_NAMESPACE_QUALIFIER asILockableSharedBool> = "int";

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

// Note that const_weakref is not supported. Seems like you can just do weakref<const T> anyway?
// TODO: I should probably clarify this with Andreas at some point.
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
    constexpr auto funcdef = ExtractAnnotation<I, Funcdef>();
    static_assert(funcdef, "asIScriptFunction* object or parameter was not given a Funcdef annotation");
    constexpr auto scope = ExtractAnnotation<funcdef->funcdef, Scope>();

    std::string r = std::string(std::meta::identifier_of(funcdef->funcdef)) + "@";

    if constexpr (std::meta::is_const(T)) { r += " const"; }
    if constexpr (scope) {
        using ScopeType = [:scope->parent:];
        const auto scopeType = std::string(TypeName<ScopeType>);
        r = scopeType + "::" + r;
    } else {
    }

    return std::define_static_string(r);
}

template <std::meta::info T> constexpr bool IsObjectHandle() {
    return T == ^^AS_NAMESPACE_QUALIFIER asIScriptObject* || T == ^^AS_NAMESPACE_QUALIFIER asIScriptObject* const;
}

template <std::meta::info T, std::meta::info I> consteval std::string_view GetInterface() {
    constexpr auto interface = ExtractAnnotation<I, Interface>();
    static_assert(interface, "asIScriptObject* object or parameter was not given an Interface annotation");
    std::string r = std::string(std::meta::identifier_of(interface->interface)) + "@";
    if constexpr (std::meta::is_const(T)) { r += " const"; }
    return std::define_static_string(r);
}

template <std::meta::info I> consteval std::string_view GetTypeDecl() {
    // Figure out which type we should work with.
    // If I reflects a function, use its return type.
    // Otherwise use type_of().
    constexpr auto baseType = std::meta::is_function(I) ? std::meta::return_type_of(I) : std::meta::type_of(I);

    if constexpr (IsObjectHandle<baseType>()) {
        // If the base type is asIScriptObject, then use its attached interface name.
        return GetInterface<baseType, I>();
    } else if constexpr (IsFuncdefHandle<baseType>()) {
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
    if constexpr (IsObjectHandle<^^T>()) {
        return GetInterface<^^T, I>();
    } else if constexpr (IsFuncdefHandle<^^T>()) {
        return GetFuncdef<^^T, I>();
    } else {
        return GetTypeDecl<T, GetSubTypes<I>()>();
    }
}

struct NonStructuralClassInformation {
    std::meta::info type;

    std::vector<std::meta::info> bases;

    std::vector<std::meta::info> derivatives;

    std::vector<ClassMember> members;

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
consteval bool FindMembersOf(
    detail::NonStructuralClassInformation& info,
    std::vector<std::meta::info>& visited,
    bool inherited = false,
    bool mixin = false
) {
    if constexpr (HasAnnotation<C, decltype(DoNotRegister)>()) { return false; }

    for (auto v : visited) {
        if (v == C) { return false; }
    }
    visited.push_back(C);

    template for (constexpr auto m :
                  std::define_static_array(std::meta::members_of(C, std::meta::access_context::current()))) {
        if constexpr (!std::meta::is_special_member_function(m) && !std::meta::is_constructor(m)
                      && !std::meta::is_destructor(m) && !std::meta::is_type(m)) {
            const auto id = GetUniqueSignatureOfMember<m>();

            // If we've already registered this member, don't register it again.
            if (!id.empty() && !std::ranges::contains(info.memberIdentifiers, id)) {
                // If this member has an Exclude annotation attached to it, don't include it in the result.
                if (!HasAnnotation<m, decltype(DoNotRegister)>()) {
                    info.members.emplace_back(m, inherited);
                    info.memberIdentifiers.push_back(id);
                }
            }
        }
    }

    template for (constexpr auto b :
                  std::define_static_array(std::meta::bases_of(C, std::meta::access_context::current()))) {
        constexpr auto bType = std::meta::type_of(b);
        constexpr auto isMixin = HasAnnotation<bType, decltype(Mixin)>();
        const auto mixinModeEnabled = mixin || isMixin;
        if (FindMembersOf<bType>(info, visited, true, mixinModeEnabled)) {
            if (!mixinModeEnabled) { info.bases.push_back(bType); }
        }
    }

    return true;
}

template <std::meta::info C>
consteval void FindMembersRecursiveOf(
    std::vector<detail::NonStructuralClassInformation>& classes,
    std::vector<std::meta::info>& visited,
    const bool recurse,
    std::vector<std::meta::info>& subclasses
) {
    if constexpr (HasAnnotation<C, decltype(DoNotRegister)>()) { return; }

    if (!subclasses.empty()) {
        // Always add to the derivatives list, even if we've already visited C,
        // as there could be multiple derivatives of C.
        for (auto& cls : classes) {
            if (cls.type == C) {
                for (const auto& subclass : subclasses) {
                    if (std::find(cls.derivatives.begin(), cls.derivatives.end(), subclass) == cls.derivatives.end()) {
                        cls.derivatives.push_back(subclass);
                    }
                }
                break;
            }
        }
        // If this loop does not add the derivative to the list, we'll add it later.
    }

    for (auto v : visited) {
        if (v == C) { return; }
    }
    visited.push_back(C);

    classes.emplace_back(C);
    for (const auto& subclass : subclasses) { classes.back().derivatives.push_back(subclass); }

    std::vector<std::meta::info> visitedInner;
    detail::FindMembersOf<C>(classes.back(), visitedInner);

    if (recurse) {
        template for (constexpr auto b :
                      std::define_static_array(std::meta::bases_of(C, std::meta::access_context::current()))) {
            constexpr auto bType = std::meta::type_of(b);
            if constexpr (!HasAnnotation<bType, decltype(Mixin)>()) {
                subclasses.push_back(C);
                FindMembersRecursiveOf<bType>(classes, visited, recurse, subclasses);
                subclasses.pop_back();
            }
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

template <std::meta::info C> consteval StructuralSpan<const ClassInformation> GetClassHierarchy(const bool recurse) {
    static_assert(
        !HasAnnotation<C, decltype(Mixin)>(), "you cannot provide Mixin classes to GetClassHierarchy() directly"
    );

    std::vector<detail::NonStructuralClassInformation> ret;
    std::vector<std::meta::info> visited;
    std::vector<std::meta::info> subclasses;
    detail::FindMembersRecursiveOf<C>(ret, visited, recurse, subclasses);

    std::vector<ClassInformation> staticRet;
    for (const auto r : ret) {
        staticRet.emplace_back(
            r.type,
            std::define_static_array(r.bases),
            std::define_static_array(r.derivatives),
            std::define_static_array(r.members)
        );
    }
    return std::define_static_array(staticRet);
}

template <std::meta::info T> bool IsAngelScriptPodType(std::unordered_set<std::type_index> const& podTypes) {
    using Type = [:T:];
    if constexpr (IsRefType<Type>() || !std::meta::is_standard_layout_type(T)) {
        return false;
    } else {
        static constexpr auto members =
            std::define_static_array(std::meta::nonstatic_data_members_of(T, std::meta::access_context::unchecked()));
        template for (constexpr auto m : members) {
            constexpr auto mType = std::meta::type_of(m);
            using M = [:mType:];

            // For future reference: without handling unions first, it seems like one of the subsequent operations
            // (minus the struct checking) causes an ICE within GCC 16.1.0:
            // g++-16.1.0: internal compiler error: Segmentation fault signal terminated program cc1plus
            // Segmentation fault (core dumped)
            // gmake[3]: *** [tests/CMakeFiles/ReflectiveAngelScriptWrapperTests.dir/build.make:202:
            // tests/CMakeFiles/ReflectiveAngelScriptWrapperTests.dir/src/engine/EngineValueTypes.cpp.o] Error 139
            // gmake[2]: *** [CMakeFiles/Makefile2:227: tests/CMakeFiles/ReflectiveAngelScriptWrapperTests.dir/all]
            // Error 2 gmake[1]: *** [CMakeFiles/Makefile2:234:
            // tests/CMakeFiles/ReflectiveAngelScriptWrapperTests.dir/rule] Error 2 gmake: *** [Makefile:195:
            // ReflectiveAngelScriptWrapperTests] Error 2 Couldn't seem to find any existing report on this specific use
            // of anonymous union reflections, so if I have time, I should try to replicate with as little code as
            // possible. Specifically, I found that T = ^^AngelScript::CScriptDictValue was causing this.

            if constexpr (std::meta::is_union_type(mType)) {
                if (!IsAngelScriptPodType<mType>(podTypes)) { return false; }
            } else if constexpr (std::meta::is_pointer_type(mType) || std::meta::is_lvalue_reference_type(mType)
                                 || std::meta::is_rvalue_reference_type(mType)) {
                return false;
            } else if constexpr (std::meta::is_class_type(mType) && std::meta::parent_of(mType) == T) {
                // This is an instance of a nested struct. If the nested struct is POD, then T could still be POD.
                if (!IsAngelScriptPodType<mType>(podTypes)) { return false; }
            } else {
                if (!podTypes.contains(std::type_index(typeid(M)))) { return false; }
            }
        }

        static constexpr auto bases =
            std::define_static_array(std::meta::bases_of(T, std::meta::access_context::unchecked()));
        template for (constexpr auto b : bases) {
            constexpr auto bType = std::meta::type_of(b);
            if (!IsAngelScriptPodType<bType>(podTypes)) { return false; }
        }

        return true;
    }
}

namespace detail {
// Determine traits of a type for registration of value types
// Copy of asGetTypeTraits() that's also constexpr. Remove this once https://github.com/anjo76/angelscript/pull/74 ends
// up in a released version.
template <typename T> constexpr AS_NAMESPACE_QUALIFIER asUINT asGetTypeTraits() {
#if defined(_MSC_VER) || defined(_LIBCPP_TYPE_TRAITS) || (__GNUC__ >= 5)                                               \
    || (defined(__clang__) && !defined(CLANG_PRE_STANDARD))
    // MSVC, XCode/Clang, and gnuc 5+
    // C++11 compliant code
    bool hasConstructor = std::is_default_constructible<T>::value && !std::is_trivially_default_constructible<T>::value;
    bool hasDestructor = std::is_destructible<T>::value && !std::is_trivially_destructible<T>::value;
    bool hasAssignmentOperator = std::is_copy_assignable<T>::value && !std::is_trivially_copy_assignable<T>::value;
    bool hasCopyConstructor = std::is_copy_constructible<T>::value && !std::is_trivially_copy_constructible<T>::value;
#elif (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8)))                                  \
    || (defined(__clang__) && defined(CLANG_PRE_STANDARD))
    // gnuc 4.8 is using a mix of C++11 standard and pre-standard templates
    bool hasConstructor = std::is_default_constructible<T>::value && !std::has_trivial_default_constructor<T>::value;
    bool hasDestructor = std::is_destructible<T>::value && !std::is_trivially_destructible<T>::value;
    bool hasAssignmentOperator = std::is_copy_assignable<T>::value && !std::has_trivial_copy_assign<T>::value;
    bool hasCopyConstructor = std::is_copy_constructible<T>::value && !std::has_trivial_copy_constructor<T>::value;
#else
    // All other compilers and versions are assumed to use non C++11 compliant code until proven otherwise
    // Not fully C++11 compliant. The has_trivial checks were used while the standard was still
    // being elaborated, but were then removed in favor of the above is_trivially checks
    // http://stackoverflow.com/questions/12702103/writing-code-that-works-when-has-trivial-destructor-is-defined-instead-of-is
    // https://github.com/mozart/mozart2/issues/51
    bool hasConstructor = std::is_default_constructible<T>::value && !std::has_trivial_default_constructor<T>::value;
    bool hasDestructor = std::is_destructible<T>::value && !std::has_trivial_destructor<T>::value;
    bool hasAssignmentOperator = std::is_copy_assignable<T>::value && !std::has_trivial_copy_assign<T>::value;
    bool hasCopyConstructor = std::is_copy_constructible<T>::value && !std::has_trivial_copy_constructor<T>::value;
#endif
    bool isFloat = std::is_floating_point<T>::value;
    bool isPrimitive = std::is_integral<T>::value || std::is_pointer<T>::value || std::is_enum<T>::value;
    bool isClass = std::is_class<T>::value;
    bool isArray = std::is_array<T>::value;

    if (isFloat) return asOBJ_APP_FLOAT;
    if (isPrimitive) return asOBJ_APP_PRIMITIVE;

    if (isClass) {
        asDWORD flags = asOBJ_APP_CLASS;
        if (hasConstructor) flags |= asOBJ_APP_CLASS_CONSTRUCTOR;
        if (hasDestructor) flags |= asOBJ_APP_CLASS_DESTRUCTOR;
        if (hasAssignmentOperator) flags |= asOBJ_APP_CLASS_ASSIGNMENT;
        if (hasCopyConstructor) flags |= asOBJ_APP_CLASS_COPY_CONSTRUCTOR;
        return flags;
    }

    if (isArray) return asOBJ_APP_ARRAY;

    // Unknown type traits
    return 0;
}

struct ValueTypeTraitsInfo {
    bool allInts = true;

    bool allFloats = true;

    bool empty = false;

    AS_NAMESPACE_QUALIFIER asUINT flags = 0;
};

template <std::meta::info T, bool ConstructorCheck = true> consteval ValueTypeTraitsInfo GetValueTypeTraitsInfo() {
    ValueTypeTraitsInfo info;

    static constexpr auto members =
        std::define_static_array(std::meta::members_of(T, std::meta::access_context::unchecked()));

    bool checkedOneNonStaticDataMember = false;

    template for (constexpr auto m : members) {
        if constexpr (ConstructorCheck && std::meta::is_constructor(m) && !std::meta::is_default_constructor(m)
                      && !std::meta::is_copy_constructor(m) && !std::meta::is_move_constructor(m)) {
            info.flags |= AS_NAMESPACE_QUALIFIER asOBJ_APP_CLASS_MORE_CONSTRUCTORS;
        }

        if constexpr (!std::meta::is_static_member(m) && !std::meta::is_function(m) && !std::meta::is_constructor(m)
                      && !std::meta::is_destructor(m) && !std::meta::is_type(m) && !std::meta::is_template(m)) {
            constexpr auto mType = std::meta::type_of(m);

            if constexpr (std::meta::is_union_type(mType)) {
                constexpr auto recursiveInfo = GetValueTypeTraitsInfo<mType, false>();
                if (!recursiveInfo.allInts) { info.allInts = false; }
                if (!recursiveInfo.allFloats) { info.allFloats = false; }
                info.flags |= recursiveInfo.flags | AS_NAMESPACE_QUALIFIER asOBJ_APP_CLASS_UNION;
                continue;
            }
            // CDateTime technically only has an int within its chrono time_point, but it's not registered with this
            // flag, so I don't think those ALLINTS and ALLFLOATS flags work recursively.
            // } else if constexpr (std::meta::is_class_type(mType)) {
            //     constexpr auto recursiveInfo = GetValueTypeTraitsInfo<mType, false>();
            //     if (!recursiveInfo.allInts) { info.allInts = false; }
            //     if (!recursiveInfo.allFloats) { info.allFloats = false; }
            //     info.flags |= recursiveInfo.flags;
            //     if constexpr (std::meta::alignment_of(m) >= 8) {
            //         info.flags |= AS_NAMESPACE_QUALIFIER asOBJ_APP_CLASS_ALIGN8;
            //     }
            //     continue;
            // }

            if constexpr (std::meta::alignment_of(m) >= 8) {
                info.flags |= AS_NAMESPACE_QUALIFIER asOBJ_APP_CLASS_ALIGN8;
            }

            if constexpr (!std::meta::is_floating_point_type(mType)) { info.allFloats = false; }

            if constexpr (!std::meta::is_pointer_type(mType) && !std::meta::is_integral_type(mType)) {
                info.allInts = false;
            }

            checkedOneNonStaticDataMember = true;
        }
    }

    if (!checkedOneNonStaticDataMember) {
        info.allInts = false;
        info.allFloats = false;
    }

    return info;
}
} // namespace detail

template <std::meta::info T> consteval AS_NAMESPACE_QUALIFIER asEObjTypeFlags GetValueTypeTraits() {
    using Type = [:T:];

    if constexpr (IsRefType<Type>()) {
        return static_cast<AS_NAMESPACE_QUALIFIER asEObjTypeFlags>(0);
    } else {
        AS_NAMESPACE_QUALIFIER asUINT flags = detail::asGetTypeTraits<Type>();

        // Now inform type traits that can't be informed from asGetTypeTraits():
        const auto info = detail::GetValueTypeTraitsInfo<T>();

        flags |= info.flags;
        if (info.allInts) { flags |= AS_NAMESPACE_QUALIFIER asOBJ_APP_CLASS_ALLINTS; }
        if (info.allFloats) { flags |= AS_NAMESPACE_QUALIFIER asOBJ_APP_CLASS_ALLFLOATS; }

        return static_cast<AS_NAMESPACE_QUALIFIER asEObjTypeFlags>(flags);
    }
}
} // namespace as
