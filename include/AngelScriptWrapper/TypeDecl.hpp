/**
 * @file TypeDecl.hpp
 * Defines functions that resolve typenames to their AngelScript equivalents.
 */

#pragma once

#include <angelscript.h>
#include <AngelScriptWrapper/Annotations.hpp>
#include <AngelScriptWrapper/Concepts.hpp>
#include <cstdint>
#include <meta>
#include <ranges>
#include <string>
#include <type_traits>

namespace as {
/**
 * If a given C++ reflection has a Rename annotation attached to it, returns its string view.
 * Otherwise returns the C++ entity's identifier.
 *
 * For classes, you will want to use the TypeName template variable instead of this function, as this function ignores
 * any specializations of TypeName.
 * @tparam O The meta info object of the C++ entity to find the AngelScript identifier of.
 * @return The C++ entity's intended AngelScript identifier.
 */
template <std::meta::info O> consteval std::string_view GetIdentifierOf();

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
template <typename T> constexpr std::string_view TypeName = as::GetIdentifierOf<^^T>();

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
 * Determines if a given typename should use a handle or a reference.
 * @tparam T The type to test. This function will test the underlying type, i.e. you could pass T* here, as well.
 * @tparam C If the type is a RefType, set this flag to true to make the handle constant. Does nothing if the type is a
 *         value type.
 * @return "@" or "@ const" if the given type has been annotated with RefType, "&" otherwise.
 */
template <typename T, bool C = false> constexpr std::string_view GetRefType();
} // namespace as

#include <AngelScriptWrapper/TypeDecl.tpp>

namespace as {
/**
 * Contains commonly-used subtype lists.
 * Application developers are encouraged to define their own commonly-used subtype lists for reuse.
 */
namespace subtype {
constexpr auto Int8 = SubTypeList<std::int8_t>();
constexpr auto Uint8 = SubTypeList<std::uint8_t>();
constexpr auto Int16 = SubTypeList<std::int16_t>();
constexpr auto Uint16 = SubTypeList<std::uint16_t>();
constexpr auto Int32 = SubTypeList<std::int32_t>();
constexpr auto Uint32 = SubTypeList<std::uint32_t>();
constexpr auto Int64 = SubTypeList<std::int64_t>();
constexpr auto Uint64 = SubTypeList<std::uint64_t>();
constexpr auto Float = SubTypeList<float>();
constexpr auto Double = SubTypeList<double>();
constexpr auto Bool = SubTypeList<bool>();
constexpr auto String = SubTypeList<std::string>();
} // namespace subtype
} // namespace as
