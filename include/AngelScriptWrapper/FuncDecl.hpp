/**
 * @file FuncDecl.hpp
 * Defines functions that resolve function signatures to their AngelScript equivalents.
 */

#pragma once

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
 * Statically computes the equivalent AngelScript declaration for a given C++ function.
 * @tparam F The std::meta::info object of the function you want to generate the declaration of.
 * @return The AngelScript declaration (with qualifiers) of the given C++ function signature.
 */
template <std::meta::info F> constexpr std::string_view GetFuncDecl();
} // namespace as

#include <AngelScriptWrapper/FuncDecl.tpp>
