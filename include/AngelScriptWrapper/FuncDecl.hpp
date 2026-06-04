/**
 * @file FuncDecl.hpp
 * Defines functions that resolve function signatures to their AngelScript equivalents.
 */

#pragma once

#include <angelscript.h>
#include <AngelScriptWrapper/Concepts.hpp>
#include <meta>
#include <string>

namespace as {
inline constexpr struct {
} Auto{};

inline constexpr struct {
} NonAuto{};

/**
 * Annotation attached to function parameters that are to be registered with a default value.
 * Compilation will fail if you attach more than one of these to a single parameter.
 */
struct DefaultsTo {
    /**
     * The default value to render into an AngelScript function parameter declaration, verbatim.
     */
    const char* value;
};

/**
 * Use this helper function to assign a default value to a function parameter in AngelScript.
 * @param val The value to render into the function declaration, verbatim. This means that if you want to assign a
 *        string default value, you must surround the value with double quotes!
 * @return The annotation to apply to the parameter.
 */
inline consteval DefaultsTo DefVal(std::string const& val) {
    constexpr auto v = std::meta::variable_of(std::meta::parameters_of(^^DefVal)[0]);
    return DefaultsTo(std::define_static_string(val));
}

/**
 * Annotation attached to functions to tell the library that it uses the CDecl call convention.
 * Trying to attach this annotation to ineligible functions will cause a compiler error (e.g. attaching to non-static
 * class methods, attaching to functions that use the generic call convention, etc.).
 * @sa EngineOptions::CallConventionDefault.
 */
inline constexpr struct {
} CDecl{};

/**
 * Annotation attached to functions to tell the library that it uses the StdCall call convention.
 * Trying to attach this annotation to ineligible functions will cause a compiler error (e.g. attaching to non-static
 * class methods, attaching to functions that use the generic call convention, etc.).
 * @sa EngineOptions::CallConventionDefault.
 */
inline constexpr struct {
} StdCall{};

/**
 * Annotation attached to functions to tell the library that it uses the _OBJFIRST variant of whatever calling
 * convention is determined.
 */
inline constexpr struct {
} ObjFirst{};

/**
 * Annotation attached to functions to tell the library that it uses the _OBJLAST variant of whatever calling
 * convention is determined.
 */
inline constexpr struct {
} ObjLast{};

/**
 * Tries to statically compute what base call convention a given function uses.
 * This function is able to determine if a function uses ThisCall or AngelScript's Generic. It is also able to pull the
 * CDecl and StdCall annotations, and if either exist, the relevant AngelScript constant will be returned. Moreover, the
 * ObjFirst and ObjLast annotations will also be factored into the result.
 * @tparam F The std::meta::info object of the function you want to get the call convention of.
 * @tparam Fallback Either asCALL_DECL or asCALL_STDCALL. If a call convention can't be derived from the given function,
 *         then this call convention will be returned. If it is asCALL_CDECL, ObjFirst and ObjLast will still be
 *         factored into the result if those annotations could be extracted.
 * @return The AngelScript call convention constant.
 */
template <std::meta::info F, AS_NAMESPACE_QUALIFIER asDWORD Fallback> constexpr int FuncCallConv();

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
