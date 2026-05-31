/**
 * @file StructuralSpan.hpp
 * Defines a version of std::span that is structural and therefore can be used in constant expressions.
 */

#pragma once

namespace as {
/**
 * Structural span.
 * Partially written by Claude Code.
 */
template <typename T> struct StructuralSpan {
    const T* ptr;
    std::size_t m_size;

    constexpr const T* begin() const {
        return ptr;
    }

    constexpr const T* end() const {
        return ptr + m_size;
    }

    constexpr const T& operator[](std::size_t i) const {
        return ptr[i];
    }

    // Implicit conversion from std::span for convenience
    constexpr StructuralSpan(std::span<const T> s) : ptr(s.data()), m_size(s.size()) {}

    constexpr StructuralSpan() : ptr(nullptr), m_size(0) {}

    constexpr std::size_t size() const {
        return m_size;
    }

    constexpr bool empty() const {
        return m_size == 0;
    }
};
} // namespace as
