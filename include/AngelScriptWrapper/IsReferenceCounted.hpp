/**
 * @file IsReferenceCounted.hpp
 * Defines the concept for AngelScript-based, reference-counted types.
 */

#pragma once

#include <concepts>

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
} // namespace as
