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
} // namespace as
