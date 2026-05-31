/**
 * @file TypeDecl.hpp
 * Defines functions that resolve typenames to their AngelScript equivalents.
 */

#pragma once

#include <angelscript.h>
#include <AngelScriptWrapper/Concepts.hpp>
#include <AngelScriptWrapper/StructuralSpan.hpp>
#include <cstdint>
#include <meta>
#include <ranges>
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
 * Annotation attached to variables, fields and function parameters that stores a list of subtypes used in an
 * AngelScript template specialization.
 * If it is attached to a function, the subtypes will be applied to the function's return type.
 * Both vectors are guaranteed to be equal in length, even if entries in recursiveSub are empty.
 */
struct TmplSubTypes {
    /**
     * A list of meta info objects for each subtype, from left to right.
     */
    StructuralSpan<const std::meta::info> subTypes;

    /**
     * If a subtype in the list itself has subtypes, they must be assigned to the corresponding element in this list.
     */
    StructuralSpan<const TmplSubTypes> recursiveSub;
};

/**
 * Variant of GetTypeDecl() used for AngelScript template types.
 * @tparam T The C++ type that is registered as a template type in the AngelScript application interface.
 * @tparam S The annotation object containing the subtypes to include in the AngelScript type declaration, if any.
 * @return The AngelScript declaration of a specialized template type.
 */
template <typename T, TmplSubTypes S> consteval std::string GetTypeDecl();

/**
 * Variant of GetTypeDecl() used for AngelScript template types.
 * @tparam T The C++ type that is registered as a template type in the AngelScript application interface.
 * @tparam S The meta info object containing the subtypes to include in the AngelScript type declaration, if any.
 * @return The AngelScript declaration of a specialized template type.
 */
template <typename T, std::meta::info S> constexpr std::string GetTypeDecl();

/**
 * Variant of GetTypeDecl() used for AngelScript template types.
 * @tparam S The C++ object that stores a variable of the template type you want to convert into an AngelScript
 *           declaration. The C++ object should also include an annotation containing the subtypes to include in the
 *           AngelScript type declaration, if any.
 * @return The AngelScript declaration of a specialized template type.
 */
template <std::meta::info S> constexpr std::string GetTypeDecl();

/**
 * Use this struct in the template parameter list of SubType[s]() when you need to declare a template type whose
 * subtypes are also template types.
 * @tparam B The C++ type that represents the AngelScript template type.
 * @tparam S The subtypes to instantiate the template type with.
 */
template <typename B, typename... S> struct Tmpl {};

/**
 * Use this function to annotate a variable that is intended to store an AngelScript object of a specialized template
 * type.
 * @tparam Ts... The subtypes of a template type instance.
 * @return The annotation to attach to e.g. a CScriptArray variable or parameter to tell the library which
 *         specialization you expect from scripts.
 * @sa Tmpl
 */
template <typename... Ts> consteval TmplSubTypes SubType();

/**
 * Alias for SubType.
 * @sa SubType().
 */
template <typename... Ts> consteval TmplSubTypes SubTypes();

/**
 * Retrieves a variable's or parameter's subtypes.
 * @tparam I The meta info of the parameter or variable to extract the subtypes of.
 * @return The list of subtypes. An empty list if the meta info object has no subtypes.
 */
template <std::meta::info I> consteval TmplSubTypes GetSubTypes();

/**
 * Annotation attached to classes to denote that they are reference types.
 * If a class is not annotated with this object, the library assumes it is a value type.
 */
inline constexpr struct {
} RefType{};

/**
 * Finds out if the developer has marked a C++ type as being a reference type in AngelScript.
 * @tparam T The type to test. Aliases are dealised. The base type will be tested, e.g. if T* is passed, the pointer
 *         will be dropped. CV qualifiers are also dropped.
 * @return True if the type has been marked as a ref type, false if the type is a value type.
 */
template <typename T> constexpr bool IsRefType();

/**
 * Annotation attached to functions to tell the library what call convention to use.
 * In most contexts, the library will default to either asCALL_CDECL or asCALL_THISCALL where appropriate. If you want
 * to use other types of call convention, you will usually have to specify them manually. Note that you must only attach
 * one CallConv annotation to a function.
 */
struct CallConv {
    /**
     * The call convention AngelScript should use for a given function or method.
     */
    AS_NAMESPACE_QUALIFIER asDWORD is;
};

/**
 * Retrieves a C++ function or method's call convention as specified via a CallConv annotation.
 * If a call convention is not specified via an annotation, this function will either return asCALL_CDECL or
 * asCALL_THISCALL depending on if the function is a non-static member of a class or not.
 * @tparam F The meta info object of the function to inspect.
 * @return The function's call convention.
 */
template <std::meta::info F> constexpr AS_NAMESPACE_QUALIFIER asDWORD GetFuncCallConv();

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
