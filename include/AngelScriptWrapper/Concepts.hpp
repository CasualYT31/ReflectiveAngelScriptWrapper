/**
 * @file Concepts.hpp
 * Defines general purpose concepts.
 */

#pragma once

#include <concepts>
#include <type_traits>

namespace as {
/**
 * A concept for AngelScript-based, reference-counted types.
 * @tparam T The type to test.
 */
template <typename T>
concept IsReferenceCounted = requires(T t) {
    { t.AddRef() } -> std::same_as<int>;
    { t.Release() } -> std::same_as<int>;
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
} // namespace as
