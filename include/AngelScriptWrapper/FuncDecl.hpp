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
/**
 * Annotation attached to pointer function parameters to tell the library to treat them as handles and not references.
 * By default, function parameters that are ref types are registered in the application interface as being passed by
 * reference. If you need to pass the ref object as a handle, then you will need to attach this annotation to it. Your
 * program will fail to compile if you attach this annotation to a parameter of a value type.
 */
inline constexpr struct {
} Handle{};

/**
 * Finds out if the developer has marked a C++ function parameter as being a handle parameter in AngelScript.
 * This function does not check if the type of the parameter is value or reference, it merely checks for the existence
 * of the Handle annotation.
 * @tparam T The meta info object of the parameter to test.
 * @return True if the parameter should be a handle, false otherwise.
 */
template <std::meta::info P> constexpr bool AsHandle();

// TODO: is this really necessary? Does CScriptArray& map to array&?

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
 * Annotation attached to functions to tell the library that it uses the CDecl calling convention.
 * Trying to attach this annotation to ineligible functions will cause a compiler error (e.g. attaching to non-static
 * class methods, attaching to functions that use the generic calling convention, etc.).
 * @sa Engine::SetDefaultCallingConvention().
 */
inline constexpr struct {
} CDecl{};

/**
 * Annotation attached to functions to tell the library that it uses the StdCall calling convention.
 * Trying to attach this annotation to ineligible functions will cause a compiler error (e.g. attaching to non-static
 * class methods, attaching to functions that use the generic calling convention, etc.).
 * @sa Engine::SetDefaultCallingConvention().
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
 * Tries to statically compute what base calling convention a given function uses.
 * This function is able to determine if a function uses ThisCall or AngelScript's Generic. It is also able to pull the
 * CDecl and StdCall annotations, and if either exist, the relevant AngelScript constant will be returned. Moreover, the
 * ObjFirst and ObjLast annotations will also be factored into the result.
 * @tparam F The std::meta::info object of the function you want to get the calling convention of.
 * @return The AngelScript calling convention constant, if it could be determined. < 0 if it could not be determined at
 *         compile time. -2 if an ObjFirst annotation was detected, -3 if an ObjLast annotation was detected, and -1 if
 *         neither were detected.
 */
template <std::meta::info F> constexpr int FuncCallConv();

/**
 * Statically computes the equivalent AngelScript declaration for a given C++ function.
 * @tparam F The std::meta::info object of the function you want to generate the declaration of.
 * @tparam RC Set this to true to forcibly remove the const qualifier from the declaration, if the corresponding C++
 *         method does have such a qualifier.
 * @return The AngelScript declaration (with qualifiers) of the given C++ function signature.
 */
template <std::meta::info F, bool RC = false> constexpr std::string_view GetFuncDecl();

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
