/**
 * @file FuncDecl.tpp
 * The implementations of template functions found within TypeDecl.
 */

#pragma once

#include <AngelScriptWrapper/TypeDecl.hpp>

namespace as {
namespace detail {
template <std::meta::info P, bool AutoHandleDefault> constexpr std::string_view GetAutoHandleFlag() {
    constexpr auto hasAutoAnno = std::meta::annotations_of_with_type(P, ^^decltype(Auto)).size() > 0;
    constexpr auto hasNonAutoAnno = std::meta::annotations_of_with_type(P, ^^decltype(NonAuto)).size() > 0;
    static_assert(!(hasAutoAnno && hasNonAutoAnno), "detected simultaneous use of Auto and NonAuto annotations");
    if constexpr (hasAutoAnno) {
        return "+";
    } else if constexpr (hasNonAutoAnno) {
        return "";
    } else if constexpr (AutoHandleDefault) {
        return "+";
    } else {
        return "";
    }
}

template <std::meta::info P, bool AutoHandleDefault> consteval std::string_view GetFuncTypeDecl() {
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
    using T = [:std::meta::is_function(P) ? std::meta::return_type_of(P) : std::meta::type_of(P):];
    if constexpr (IsRefType<T>()) {
        if constexpr (IsReference<T>) {
            using Tr = std::remove_reference_t<T>;
            static_assert(!IsPointer<Tr>, "detected parameter*&, which is not supported");
            return std::define_static_string(std::string(detail::OverrideTypeOf<Tr, P>) + "&");
        } else if constexpr (IsPointer<T>) {
            using Tp = std::remove_pointer_t<T>;
            static_assert(!IsPointer<Tp>, "detected parameter**, which is not supported");
            return std::define_static_string(
                std::string(detail::OverrideTypeOf<T, P>) + std::string(detail::GetAutoHandleFlag<P, AutoHandleDefault>())
            );
        } else {
            throw "reference types must be passed by reference or by pointer";
        }
    } else {
        if constexpr (IsReference<T>) {
            using Tr = std::remove_reference_t<T>;
            static_assert(!IsPointer<Tr>, "detected parameter*&, which is not supported");
            if constexpr (IsConst<Tr>) {
                return std::define_static_string(std::string(detail::OverrideTypeOf<Tr, P>) + "&in");
            } else {
                return std::define_static_string(std::string(detail::OverrideTypeOf<Tr, P>) + "&out");
            }
        } else if constexpr (IsPointer<T>) {
            using Tp = std::remove_pointer_t<T>;
            static_assert(!IsPointer<Tp>, "detected parameter**, which is not supported");
            if constexpr (IsConst<Tp>) {
                return std::define_static_string(std::string(detail::OverrideTypeOf<Tp, P>) + "&in");
            } else {
                return std::define_static_string(std::string(detail::OverrideTypeOf<Tp, P>) + "&out");
            }
        } else {
            if constexpr (std::meta::is_const(P)) {
                return std::define_static_string("const " + std::string(detail::OverrideTypeOf<T, P>));
            } else {
                return detail::OverrideTypeOf<T, P>;
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

template <std::meta::info F>
constexpr bool HasGenericCallConvSig =
    (std::meta::return_type_of(F) == ^^void) && (std::meta::parameters_of(F).size() == 1)
    && (std::meta::type_of(std::meta::parameters_of(F)[0]) == ^^AS_NAMESPACE_QUALIFIER asIScriptGeneric*);

template <std::meta::info P, typename R, bool C, typename... Ps>
consteval std::meta::info FindOverload(const std::string_view identifier) {
    // Mostly written by Claude Code.
    for (auto member : std::meta::members_of(P, std::meta::access_context::current())) {
        if (!std::meta::is_function(member)) { continue; }

        if (!std::meta::has_identifier(member)) { continue; }

        if (std::meta::identifier_of(member) != identifier) { continue; }

        if (std::meta::is_const(member) != C) { continue; }

        if (std::meta::return_type_of(member) != ^^R) { continue; }

        auto params = std::meta::parameters_of(member);

        if (params.size() != sizeof...(Ps)) { continue; }

        bool match = [&]<std::size_t... Is>(std::index_sequence<Is...>) {
            return ((std::meta::type_of(params[Is]) == ^^Ps) && ...);
        }(std::make_index_sequence<sizeof...(Ps)>{});

        if (match) { return member; }
    }
    throw "could not find overload";
}
} // namespace detail

template <std::meta::info F, AS_NAMESPACE_QUALIFIER asDWORD Fallback> constexpr int FuncCallConv() {
    static_assert(
        Fallback == AS_NAMESPACE_QUALIFIER asCALL_CDECL || Fallback == AS_NAMESPACE_QUALIFIER asCALL_STDCALL,
        "illegal fallback call convention provided, it must be either CDecl or StdCall"
    );

    constexpr auto cdeclAnnotation = !std::meta::annotations_of_with_type(F, ^^decltype(CDecl)).empty();
    constexpr auto stdcallAnnotation = !std::meta::annotations_of_with_type(F, ^^decltype(StdCall)).empty();
    static_assert(
        !(cdeclAnnotation && stdcallAnnotation),
        std::string(std::meta::display_string_of(F)) + " was given a mix of call convention annotations"
    );
    constexpr auto objFirstAnnotation = !std::meta::annotations_of_with_type(F, ^^decltype(ObjFirst)).empty();
    constexpr auto objLastAnnotation = !std::meta::annotations_of_with_type(F, ^^decltype(ObjLast)).empty();
    static_assert(
        !(objFirstAnnotation && objLastAnnotation),
        std::string(std::meta::display_string_of(F)) + " was given a mix of object placement modifier annotations"
    );
    constexpr auto hasGenericSignature = detail::HasGenericCallConvSig<F>;

    if constexpr (std::meta::is_class_member(F) && !std::meta::is_static_member(F)) {
        // First, we always check if a function is a non-static member of a class.
        // If it is, it always uses some variant of ThisCall.
        static_assert(
            !cdeclAnnotation,
            std::string(std::meta::display_string_of(F)) + " was given an invalid call convention annotation"
        );
        static_assert(
            !stdcallAnnotation,
            std::string(std::meta::display_string_of(F)) + " was given an invalid call convention annotation"
        );
        static_assert(
            !hasGenericSignature,
            std::string(std::meta::display_string_of(F))
                + " is trying to use the generic call convention, but non-static class members can't use the generic "
                  "call convention"
        );
        if (objFirstAnnotation) {
            return AS_NAMESPACE_QUALIFIER asCALL_THISCALL_OBJFIRST;
        } else if (objLastAnnotation) {
            return AS_NAMESPACE_QUALIFIER asCALL_THISCALL_OBJLAST;
        } else {
            return AS_NAMESPACE_QUALIFIER asCALL_THISCALL;
        }
    } else if constexpr (hasGenericSignature) {
        // Second, we check for the generic call convention.
        static_assert(
            !cdeclAnnotation,
            std::string(std::meta::display_string_of(F)) + " was given an invalid call convention annotation"
        );
        static_assert(
            !stdcallAnnotation,
            std::string(std::meta::display_string_of(F)) + " was given an invalid call convention annotation"
        );
        static_assert(
            !objFirstAnnotation && !objLastAnnotation,
            std::string(std::meta::display_string_of(F))
                + ": the generic call convention does not support object placement modifiers"
        );
        return AS_NAMESPACE_QUALIFIER asCALL_GENERIC;
    } else if constexpr (stdcallAnnotation) {
        // Lastly, look for CDecl and StdCall.
        static_assert(
            !objFirstAnnotation && !objLastAnnotation,
            std::string(std::meta::display_string_of(F))
                + ": the StdCall call convention does not support object placement modifiers"
        );
        return AS_NAMESPACE_QUALIFIER asCALL_STDCALL;
    } else if constexpr (cdeclAnnotation || Fallback == AS_NAMESPACE_QUALIFIER asCALL_CDECL) {
        // Either the developer explicitly requested CDecl, or the fallback is CDecl.
        if (objFirstAnnotation) {
            return AS_NAMESPACE_QUALIFIER asCALL_CDECL_OBJFIRST;
        } else if (objLastAnnotation) {
            return AS_NAMESPACE_QUALIFIER asCALL_CDECL_OBJLAST;
        } else {
            return AS_NAMESPACE_QUALIFIER asCALL_CDECL;
        }
    } else {
        // The call convention can't be deduced, use the fallback
        // (this branch will only be reached if the fallback is StdCall currently).
        static_assert(
            !objFirstAnnotation && !objLastAnnotation,
            std::string(std::meta::display_string_of(F))
                + ": the StdCall call convention does not support object placement modifiers"
        );
        return Fallback;
    }
}

template <std::meta::info F, bool AutoHandleDefault, bool RC> constexpr std::string_view GetFuncDecl() {
    std::string decl;
    // Return type.
    using RetType = [:std::meta::return_type_of(F):];
    if constexpr (IsRefType<RetType>()) {
        // Reference types use parameter logic:
        decl += std::string(detail::GetFuncTypeDecl<F, AutoHandleDefault>());
    } else {
        // Value types use regular TypeOf logic:
        decl += std::string(TypeOf<F>);
    }
    // Name.
    decl += " " + std::string(as::GetIdentifierOf<F>()) + "(";
    // Parameter list.
    template for (constexpr auto P : std::define_static_array(std::meta::parameters_of(F))) {
        decl += std::string(detail::GetFuncTypeDecl<P, AutoHandleDefault>()) + detail::DefaultValueOrEmpty<P>() + ", ";
    }
    // Remove last comma.
    if constexpr (std::meta::parameters_of(F).size() > 0) { decl.erase(decl.size() - 2); }
    // Close.
    decl += ")";
    // Is this a const method?
    if constexpr (!RC && std::meta::is_const(F)) { decl += " const"; }
    return std::define_static_string(decl);
}

template <std::meta::info P, typename R, bool C, typename... Ps>
consteval std::meta::info FindOverload(const std::string_view identifier) {
    return detail::FindOverload<P, R, C, std::decay_t<Ps>...>(identifier);
}
} // namespace as
