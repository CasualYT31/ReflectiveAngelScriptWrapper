#pragma once

#include <AngelScriptWrapper/TypeDecl.hpp>

namespace as {
namespace detail {
template <std::meta::info P, bool AutoHandleDefault> constexpr std::string_view GetAutoHandleFlag() {
    constexpr auto hasAutoAnno = HasAnnotation<P, decltype(Auto)>();
    constexpr auto hasNonAutoAnno = HasAnnotation<P, decltype(NonAuto)>();
    static_assert(
        AtLeastOneOf<hasAutoAnno, hasNonAutoAnno>(), "detected simultaneous use of Auto and NonAuto annotations"
    );
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
                std::string(detail::OverrideTypeOf<T, P>)
                + std::string(detail::GetAutoHandleFlag<P, AutoHandleDefault>())
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
    constexpr auto defVal = ExtractAnnotation<P, DefaultsTo>();
    if constexpr (defVal) {
        return " = " + std::string(defVal->value);
    } else {
        return "";
    }
}

template <std::meta::info F>
constexpr bool HasGenericCallConvSig =
    (std::meta::return_type_of(F) == ^^void) && (std::meta::parameters_of(F).size() == 1)
    && (std::meta::type_of(std::meta::parameters_of(F)[0]) == ^^AS_NAMESPACE_QUALIFIER asIScriptGeneric*);

template <std::meta::info F> constexpr CallConvResults DetectGenericCallConv() {
    constexpr auto hasGenericSignature = HasGenericCallConvSig<F>;
    constexpr auto genericAnnotation = ExtractAnnotation<F, GenericWithDecl>();
    constexpr bool annotationHasDecl = genericAnnotation && !std::string_view(genericAnnotation->decl).empty();
    constexpr auto objFirstAnnotation = HasAnnotation<F, decltype(ObjFirst)>();
    constexpr auto objLastAnnotation = HasAnnotation<F, decltype(ObjLast)>();
    static_assert(
        AtLeastOneOf<objFirstAnnotation, objLastAnnotation>(),
        std::string(std::meta::display_string_of(F)) + " was given a mix of object placement modifier annotations"
    );
    constexpr bool isNonStaticClassMember = std::meta::is_class_member(F) && !std::meta::is_static_member(F);

    CallConvResults opts;
    if constexpr (hasGenericSignature) {
        static_assert(!isNonStaticClassMember, "Non-static class methods cannot use the generic call convention");
        static_assert(
            annotationHasDecl,
            "functions that have the void(asIScriptGeneric*) signature must be given a GenericWithDecl annotation with "
            "a full AngelScript declaration"
        );

        opts.callConv = AS_NAMESPACE_QUALIFIER asCALL_GENERIC;
        opts.decl = std::string_view(genericAnnotation->decl);
        opts.genericType = GenericCallConvType::None;
        return opts;
    } else if constexpr (genericAnnotation) {
        static_assert(!isNonStaticClassMember, "Non-static class methods cannot use the generic call convention");
        static_assert(
            !annotationHasDecl,
            "an AngelScript function declaration was given to the GenericWithDecl annotation, but the annotated "
            "function did not have the void(asIScriptGeneric*) signature: please remove the declaration if you do not "
            "intend to use the void(asIScriptGeneric*) signature for your function"
        );

        opts.callConv = AS_NAMESPACE_QUALIFIER asCALL_GENERIC;
        // Don't calculate the decl in here.
        // Find out which WRAP macro should be used.
        if constexpr (std::meta::is_class_member(F)) {
            // TODO: at least this is implied from the macros in the autowrapper add on, but is this strictly true?
            static_assert(
                !objFirstAnnotation && !objLastAnnotation,
                "ObjFirst and ObjLast are not supported on static class functions for the generic call convention"
            );
            opts.genericType = GenericCallConvType::WrapMFn;
        } else if constexpr (objFirstAnnotation) {
            opts.genericType = GenericCallConvType::WrapObjFirst;
        } else if constexpr (objLastAnnotation) {
            opts.genericType = GenericCallConvType::WrapObjLast;
        } else {
            opts.genericType = GenericCallConvType::WrapFn;
        }
        return opts;
    } else {
        return opts;
    }
}

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

template <std::meta::info P> constexpr bool IsVariableParameterType() {
    return (std::meta::type_of(P) == (^^void*)) || (std::meta::type_of(P) == (^^const void*));
}

template <std::meta::info P> constexpr bool IsVariableParameterTypeId() {
    return (std::meta::type_of(P) == (^^int));
}
} // namespace detail

// TODO: the implementation of this function is not easy to follow, especially as it's split into this and
//       detail::DetectGenericCallConv. They should both be rewritten, first by combining them into one big
//       function and then splitting them up into multiple detail functions towards the end of the refactor.
template <std::meta::info F, AS_NAMESPACE_QUALIFIER asDWORD Fallback> constexpr CallConvResults FuncCallConv() {
    static_assert(
        Fallback == AS_NAMESPACE_QUALIFIER asCALL_CDECL || Fallback == AS_NAMESPACE_QUALIFIER asCALL_STDCALL
            || Fallback == AS_NAMESPACE_QUALIFIER asCALL_GENERIC,
        "illegal fallback call convention provided, it must be either CDecl, StdCall or Generic"
    );

    constexpr auto initialGenericCallConvCheck = detail::DetectGenericCallConv<F>();
    if constexpr (initialGenericCallConvCheck.callConv == AS_NAMESPACE_QUALIFIER asCALL_GENERIC) {
        return initialGenericCallConvCheck;
    } else {
        constexpr auto cdeclAnnotation = HasAnnotation<F, decltype(CDecl)>();
        constexpr auto stdcallAnnotation = HasAnnotation<F, decltype(StdCall)>();
        static_assert(
            AtLeastOneOf<cdeclAnnotation, stdcallAnnotation>(),
            std::string(std::meta::display_string_of(F)) + " was given a mix of call convention annotations"
        );
        constexpr auto objFirstAnnotation = HasAnnotation<F, decltype(ObjFirst)>();
        constexpr auto objLastAnnotation = HasAnnotation<F, decltype(ObjLast)>();
        static_assert(
            AtLeastOneOf<objFirstAnnotation, objLastAnnotation>(),
            std::string(std::meta::display_string_of(F)) + " was given a mix of object placement modifier annotations"
        );

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
            if (objFirstAnnotation) {
                return CallConvResults(AS_NAMESPACE_QUALIFIER asCALL_THISCALL_OBJFIRST);
            } else if (objLastAnnotation) {
                return CallConvResults(AS_NAMESPACE_QUALIFIER asCALL_THISCALL_OBJLAST);
            } else {
                return CallConvResults(AS_NAMESPACE_QUALIFIER asCALL_THISCALL);
            }
        } else if constexpr (stdcallAnnotation) {
            // Lastly, look for CDecl and StdCall.
            static_assert(
                !objFirstAnnotation && !objLastAnnotation,
                std::string(std::meta::display_string_of(F))
                    + ": the StdCall call convention does not support object placement modifiers"
            );
            return CallConvResults(AS_NAMESPACE_QUALIFIER asCALL_STDCALL);
        } else if constexpr (cdeclAnnotation || Fallback == AS_NAMESPACE_QUALIFIER asCALL_CDECL) {
            // Either the developer explicitly requested CDecl, or the fallback is CDecl.
            if (objFirstAnnotation) {
                return CallConvResults(AS_NAMESPACE_QUALIFIER asCALL_CDECL_OBJFIRST);
            } else if (objLastAnnotation) {
                return CallConvResults(AS_NAMESPACE_QUALIFIER asCALL_CDECL_OBJLAST);
            } else {
                return CallConvResults(AS_NAMESPACE_QUALIFIER asCALL_CDECL);
            }
        } else if constexpr (Fallback == AS_NAMESPACE_QUALIFIER asCALL_GENERIC) {
            // The fallback is Generic. It will not have the void(asIScriptGeneric*) signature.
            constexpr auto classMember = std::meta::is_class_member(F);
            CallConvResults res;
            res.callConv = AS_NAMESPACE_QUALIFIER asCALL_GENERIC;
            if constexpr (classMember) {
                // TODO: at least this is implied from the macros in the autowrapper add on, but is this strictly true?
                static_assert(
                    !objFirstAnnotation && !objLastAnnotation,
                    "ObjFirst and ObjLast are not supported on static class functions for the generic call convention"
                );
                res.genericType = GenericCallConvType::WrapMFn;
            } else if constexpr (objFirstAnnotation) {
                res.genericType = GenericCallConvType::WrapObjFirst;
            } else if constexpr (objLastAnnotation) {
                res.genericType = GenericCallConvType::WrapObjLast;
            } else {
                res.genericType = GenericCallConvType::WrapFn;
            }
            return res;
        } else {
            // The call convention can't be deduced, use the fallback
            // (this branch will only be reached if the fallback is StdCall currently).
            static_assert(
                !objFirstAnnotation && !objLastAnnotation,
                std::string(std::meta::display_string_of(F))
                    + ": the StdCall call convention does not support object placement modifiers"
            );
            return CallConvResults(AS_NAMESPACE_QUALIFIER asCALL_STDCALL);
        }
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
    // Tracks whether we're in the process of declaring a variable type in the parameter list.
    // 0 = no, 1 = &in, 2 = &out.
    static constexpr int NO = 0;
    static constexpr int IN = 1;
    static constexpr int OUT = 2;
    int variableTypeInProgress = NO;
    template for (constexpr auto P : std::define_static_array(std::meta::parameters_of(F))) {
        using PType = typename[:std::meta::type_of(P):];
        if (detail::IsVariableParameterType<P>()) {
            if (variableTypeInProgress) {
                throw("you cannot declare two void* parameters in a row; the parameter after a void* must be an int "
                      "typeId");
            }
            variableTypeInProgress = std::meta::is_const(^^std::remove_pointer_t<PType>) ? IN : OUT;
        } else if (variableTypeInProgress && detail::IsVariableParameterTypeId<P>()) {
            if (variableTypeInProgress == IN) {
                decl += "const ?&in, ";
            } else {
                decl += "?&out, ";
            }
            variableTypeInProgress = NO;
        } else {
            if (variableTypeInProgress) { throw("the parameter after a void* must be an int typeId"); }
            decl +=
                std::string(detail::GetFuncTypeDecl<P, AutoHandleDefault>()) + detail::DefaultValueOrEmpty<P>() + ", ";
        }
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
