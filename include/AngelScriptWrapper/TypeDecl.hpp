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
 * Annotation attached to variables, fields and function parameters that stores a list of subtypes used in an
 * AngelScript template specialization.
 * If it is attached to a function, the subtypes will be applied to the function's return type.
 * Both vectors are guaranteed to be equal in length, even if entries in recursiveSub are empty.
 * @sa SubTypeList()
 */
struct SubTypes {
    /**
     * A list of meta info objects for each subtype, from left to right.
     */
    StructuralSpan<const std::meta::info> subTypes;

    /**
     * If a subtype in the list itself has subtypes, they must be assigned to the corresponding element in this list.
     */
    StructuralSpan<const SubTypes> recursiveSub;
};

namespace detail {
template <typename T, SubTypes S> consteval std::string_view GetTypeDecl();
template <std::meta::info I> consteval std::string_view GetTypeDecl();
template <typename T, std::meta::info I> consteval std::string_view GetTypeDecl();

/**
 * Version of TypeOf that's used when you want to use a different type than the type of the object reflected by I.
 * Subtypes attached to I are still respected. Reserved for internal use only.
 */
template <typename T, std::meta::info I> constexpr std::string_view OverrideTypeOf = detail::GetTypeDecl<T, I>();
} // namespace detail

/**
 * Stores the AngelScript typename of the given C++ type.
 * Handles const qualifiers and pointers, though pointers are not handled recursively! This limitation is acceptable
 * since AngelScript does not allow handles to handles or references to references.
 * @tparam T The C++ type whose AngelScript type is to be deduced.
 */
template <typename T> constexpr std::string_view TypeName = std::meta::identifier_of(^^T);

/**
 * Stores the qualified AngelScript typename of the given C++ type that represents a template type.
 * @tparam T The base C++ type that represents a template AngelScript type.
 * @tparam S The subtypes assigned to the template type.
 * @sa SubTypeList().
 */
template <typename T, SubTypes S> constexpr std::string_view TmplTypeName = detail::GetTypeDecl<T, S>();

/**
 * Stores the qualified AngelScript typename of the given C++ entity.
 * You can provide a reflected variable, function parameter, or function to I. If a function is given, the return type
 * will be stored.
 * @tparam I The meta info object of the entity whose AngelScript type is to be stored.
 */
template <std::meta::info I> constexpr std::string_view TypeOf = detail::GetTypeDecl<I>();

/**
 * Use this struct in the template parameter list of SubTypeList() when you need to declare a template type whose
 * subtypes are also template types.
 * @tparam B The C++ type that represents the AngelScript template type.
 * @tparam S The subtypes to instantiate the template type with.
 */
template <typename B, typename... S> struct Tmpl {};

/**
 * Used to generate a SubTypes annotation at compile time.
 * @tparam Ts... The subtypes of a template type instance.
 * @return The annotation to attach to e.g. a CScriptArray variable, parameter or return value to tell the library which
 *         specialization you expect from scripts.
 * @sa Tmpl
 */
template <typename... Ts> consteval SubTypes SubTypeList();

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
 * Determines if a given typename should use a handle or a reference.
 * @tparam T The type to test. This function will test the underlying type, i.e. you could pass T* here, as well.
 * @tparam C If the type is a RefType, set this flag to true to make the handle constant. Does nothing if the type is a
 *         value type.
 * @return "@" or "@ const" if the given type has been annotated with RefType, "&" otherwise.
 */
template <typename T, bool C = false> constexpr std::string_view GetRefType();

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
} // namespace as

#include <AngelScriptWrapper/TypeDecl.tpp>
