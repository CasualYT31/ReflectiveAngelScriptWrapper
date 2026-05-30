/**
 * @file TypeDecl.hpp
 * Defines functions that resolve typenames to their AngelScript equivalents.
 */

#pragma once

#include <AngelScriptWrapper/Concepts.hpp>
#include <cstdint>
#include <meta>
#include <string>
#include <type_traits>

namespace as {
/**
 * Statically computes the equivalent AngelScript type declaration for a given C++ type.
 * This function will automatically handle const qualifiers and handles for you. Primitive types have their own
 * specializations. For all other types, the function will grab the type's identifier using std::meta and use that. If
 * you wish to have a different name for your type in the AngelScript interface, you will need to specialize this
 * template function before attempting to work with the type with an Engine wrapper. E.g.:
 * @code{.cpp}
 * template <> constexpr std::string as::GetTypeDecl<CScriptArray>() {
 *     return "array";
 * }
 * static_assert(GetTypeDecl<CScriptArray>() == "array");
 * @endcode
 * This is just an illustrative example: this library already provides the specialization for all AngelScript types,
 * primitive or add-on (except for const_weakref).
 * @tparam T The C++ type to find the AngelScript name/declaration of.
 * @return The AngelScript name (with qualifiers where given) of the given C++ type.
 */
template <typename T> constexpr std::string GetTypeDecl();

/**
 * Variant of GetTypeDecl() used for AngelScript template types.
 * AngelScript template types, such as CScriptArray, store their subtype information at runtime. This makes it
 * impossible for you to tell the C++ compiler what you intend to store inside a given CScriptArray instance.
 *
 * The solution this library implements involves type aliases. Reflection in C++ treats types and type aliases as
 * separate entities, so we can define a type alias for every specialization of an AngelScript template type that you
 * expect to work with in your application interface.
 *
 * Unfortunately, it's not as simple as passing in the type alias into the regular version of GetTypeDecl(), because by
 * the time the compiler has reached the function, the type alias has been resolved and we're stuck with the original
 * type. So you will need to provide the std::meta::info object of the type alias directly to the GetTypeDecl() function
 * so it can resolve the alias and not the type the alias points to.
 *
 * Here is an example of how to get CScriptArray working as intended:
 * @code{.cpp}
 * // First, define the specialization for the base array typename:
 * template <> constexpr std::string as::GetTypeDecl<CScriptArray>() {
 *     return "array";
 * }
 *
 * // Then, specialize the CScriptArray for strings, as an example:
 * using StringArray = Specialize<CScriptArray, std::string>;
 *
 * // Now you can generate array<string>:
 * static_assert(GetTypeDecl<^^StringArray::T>() == "array<string>");
 *
 * // Slight downside is that you will need to define type aliases for all the variants you need, too:
 * using StringArrayHandle = Specialize<CScriptArray*, std::string>;
 * using StringArrayConstHandle = Specialize<CScriptArray* const, std::string>;
 * using StringConstArray = Specialize<const CScriptArray, std::string>;
 * using ConstStringConstArrayConstHandle = Specialize<const CScriptArray* const, const std::string>;
 * // etc.
 * @endcode
 * @tparam Alias The std::meta::info object of the alias generated using the Specialize template struct.
 * @return The AngelScript name (with qualifiers where given) of the given C++ type.
 * @sa as::Specialize
 */
template <std::meta::info Alias> constexpr std::string GetTypeDecl();

/**
 * Annotation attached to classes to denote that they are reference types.
 */
inline constexpr struct {
} RefType{};

/**
 * Annotation attached to classes to denote that they are value types.
 * This will be the default type if RefType is not specified.
 */
inline constexpr struct {
} ValueType{};

/**
 * Determines if a given typename should use a handle or a reference.
 * @tparam T The type to test. This function will test the underlying type, i.e. you could pass T* here, as well.
 * @tparam C If the type is a RefType, set this flag to true to make the handle constant. Does nothing if the type is a
 *         value type.
 * @return "@" or "@ const" if the given type has been annotated with RefType, "&" otherwise.
 */
template <typename T, bool C = false> constexpr std::string GetRefType();
} // namespace as

#include <AngelScriptWrapper/TypeDecl.tpp>

namespace as {
/**
 * A helper struct to specialize template type declarations in AngelScript.
 * See the documentation on GetTypeDecl<T, Alias>() for more details and an example.
 * TODO: to support more than one subtype, it's easier to just define a Specialize struct that takes three template
 *       parameters, or four, etc.
 * @tparam B The C++ type, registered as an AngelScript template type, that is being specialized.
 * @tparam T The subtype to specialize with.
 */
template <typename B, typename S> struct Specialize final {
    /**
     * Alias of the C++ class that implements the AngelScript template type.
     * You must use this alias throughout declarations that directly touch the AngelScript application interface.
     */
    using T = B;

    /**
     * A custom GetTypeDecl() implementation that GetTypDecl<T, Alias>() detects and invokes instead of resolving the
     * type declaration of T on its own.
     */
    static constexpr std::string GetTypeDecl();
};
} // namespace as

#include <AngelScriptWrapper/Specialize.tpp>
