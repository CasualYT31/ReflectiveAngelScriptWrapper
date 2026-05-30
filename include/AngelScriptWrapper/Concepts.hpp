/**
 * @file Concepts.hpp
 * Defines general purpose concepts.
 */

#pragma once

#include <concepts>
#include <string>
#include <type_traits>

namespace as {
/**
 * A concept for AngelScript-based, reference-counted types.
 * @tparam T The type to test.
 */
template <typename T>
concept IsReferenceCounted = requires(T t) {
    t.AddRef();
    t.Release();
};

/**
 * A concept for arithmetic types.
 * @tparam T The type to test.
 */
template <typename T>
concept IsArithmetic = std::is_arithmetic_v<T>;

/**
 * A concept for const types.
 * @tparam T The type to test.
 */
template <typename T>
concept IsConst = std::is_const_v<T>;

/**
 * A concept for pointer types.
 * @tparam T the type to test.
 */
template <typename T>
concept IsPointer = std::is_pointer_v<T>;

/**
 * A concept for types that define a custom AngelScript type declaration.
 * @tparam T The type to test.
 */
template <typename T>
concept HasCustomTypeDecl = requires {
    { T::GetTypeDecl() } -> std::same_as<std::string>;
};

/**
 * Removes pointers from a type recursively.
 * https://stackoverflow.com/a/39492671
 * @tparam T The type to recursively remove all pointers from.
 */
template <typename T>
struct remove_all_pointers
    : std::conditional_t<std::is_pointer_v<T>, remove_all_pointers<std::remove_pointer_t<T>>, std::type_identity<T>> {};

/**
 * The type alias representing the given type without any pointers.
 * @tparam T The type to recursively remove all pointers from.
 */
template <typename T> using remove_all_pointers_t = typename remove_all_pointers<T>::type;
} // namespace as
