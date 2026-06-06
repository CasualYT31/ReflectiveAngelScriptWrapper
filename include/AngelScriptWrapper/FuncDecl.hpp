/**
 * @file FuncDecl.hpp
 * Defines functions that resolve function signatures to their AngelScript equivalents.
 */

#pragma once

#include <angelscript.h>
#include <AngelScriptWrapper/Annotations.hpp>
#include <AngelScriptWrapper/Concepts.hpp>
#include <meta>
#include <string>

namespace as {
/**
 * The results of call convention deduction.
 */
struct CallConvResults {
    /**
     * The call convention deduced at compile time.
     */
    AS_NAMESPACE_QUALIFIER asDWORD callConv = AS_NAMESPACE_QUALIFIER asCALL_CDECL;

    /**
     * The declaration to register the function with, if it was explicitly provided in a Generic annotation.
     */
    std::string_view decl = "";

    /**
     * The type of macro to use to generate the wrapper function for the original C++ function, in case the callConv is
     * asCALL_GENERIC.
     */
    GenericCallConvType genericType = GenericCallConvType::None;
};

/**
 * Tries to statically compute what base call convention a given function uses.
 * This function is able to determine if a function uses ThisCall, but not AngelScript's Generic convention (use
 * DetectGenericCallConv() for that instead). It is also able to pull the CDecl and StdCall annotations, and if either
 * exist, the relevant AngelScript constant will be returned. Moreover, the ObjFirst and ObjLast annotations will also
 * be factored into the result.
 * @tparam F The std::meta::info object of the function you want to get the call convention of.
 * @tparam Fallback Either asCALL_DECL or asCALL_STDCALL. If a call convention can't be derived from the given function,
 *         then this call convention will be returned. If it is asCALL_CDECL, ObjFirst and ObjLast will still be
 *         factored into the result if those annotations could be extracted.
 * @return The AngelScript call convention. If the generic call convention was detected, additional information will be
 *         included, such as which autowrapper macro should be used on the function object, if any (as it doesn't use
 *         the void(asIScriptGeneric*) signature), and if an AngelScript declaration was given directly by the
 *         developer (as it <em>does</em> use the aforementioned signature and so it can't be computed via reflection
 *         normally).
 * @sa DetectGenericCallConv().
 */
template <std::meta::info F, AS_NAMESPACE_QUALIFIER asDWORD Fallback> constexpr CallConvResults FuncCallConv();

/**
 * Statically computes the equivalent AngelScript declaration for a given C++ function.
 * @tparam F The std::meta::info object of the function you want to generate the declaration of.
 * @tparam AutoHandleDefault For any handle parameters found, default to non-auto handles (false) or auto handles
 *         (true), if the parameter does not have an explicit Auto or NonAuto annotation.
 * @tparam RC Set this to true to forcibly remove the const qualifier from the declaration, if the corresponding C++
 *         method does have such a qualifier.
 * @return The AngelScript declaration (with qualifiers) of the given C++ function signature.
 */
template <std::meta::info F, bool AutoHandleDefault = false, bool RC = false> constexpr std::string_view GetFuncDecl();

/**
 * Version of the Generic() helper function that generates an AngelScript function declaration for a generic call
 * convention function from a C++ function declaration.
 * @tparam F The std::meta::info object of the function you want to generate the declaration of. Note that the name of
 *         the generic function will become whatever you named this function!
 * @tparam AutoHandleDefault Passed directly to GetFuncDecl().
 * @tparam RC Passed directly to GetFuncDecl().
 * @return The generic call convention annotation to attach to the actual function you want to register.
 */
template <std::meta::info F, bool AutoHandleDefault = false, bool RC = false> inline consteval GenericWithDecl Generic() {
    return GenericWithDecl{ std::define_static_string(GetFuncDecl<F, AutoHandleDefault, RC>()) };
}

/**
 * Helper function that resolves overload sets.
 * You cannot currently take the reflection of functions or methods that are overloaded directly. Developers will need
 * to explicitly provide the return type and parameters of a function to this function so it can iterate over the
 * members of a parent reflection to find the intended function reflection.
 * @tparam P A reflection of the parent of the function to reflect, e.g. class or namespace.
 * @tparam R The return type of the function to reflect.
 * @tparam C Whether the method has been qualified as const (true) or not (false). Functions always have no const
 *         qualifiers.
 * @tparam Ps The types of the parameters of the function to reflect. Type adjustment will be carried out for you.
 * @param identifier The identifier of the function or method to search for.
 * @return A reflection of the overloaded function or method.
 */
template <std::meta::info P, typename R, bool C, typename... Ps>
consteval std::meta::info FindOverload(const std::string_view identifier);
} // namespace as

#include <AngelScriptWrapper/FuncDecl.tpp>
