/**
 * @file ReferenceType.hpp
 * Defines a base class you can inherit from that provides reference counting for you.
 */

#pragma once

#include <angelscript.h>

namespace as {
/**
 * Base class containing standard reference counting logic.
 * @tparam T Subclasses need to provide their type [all the way] up to ReferenceType.
 */
template <typename T> struct ReferenceType {
    virtual ~ReferenceType() noexcept = default;

    /**
     * Increments the reference counter.
     */
    inline void AddRef() const noexcept {
        AS_NAMESPACE_QUALIFIER asAtomicInc(m_referenceCounter);
    }

    /**
     * Decrements the reference counter, deleting this once the reference counter hits 0.
     */
    inline void Release() const noexcept {
        if (AS_NAMESPACE_QUALIFIER asAtomicDec(m_referenceCounter) <= 0) { delete static_cast<const T*>(this); }
    }

    /**
     * Returns the reference counter.
     */
    inline int RefCount() const noexcept {
        return m_referenceCounter;
    }

protected:
    /**
     * ReferenceType can only be inherited from.
     */
    ReferenceType() = default;

private:
    /**
     * The reference counter.
     */
    mutable int m_referenceCounter = 1;
};

template <typename T> struct ReferenceTypeWithDefaultFactory : public ReferenceType<T> {
    static inline T* Factory() {
        return new T();
    }
};
} // namespace as
