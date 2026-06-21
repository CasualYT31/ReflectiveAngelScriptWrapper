/**
 * @file ReferenceType.hpp
 * Defines a base class you can inherit from that provides reference counting for you.
 */

#pragma once

#include <angelscript.h>

namespace as {
/**
 * Base class containing standard reference counting logic.
 */
struct[[= as::Mixin]] ReferenceType {
    /**
     * Don't forget that if you intend to introduce a base class of your own that inherits from ReferenceType, you
     * @b must make its destructor virtual.
     */
    virtual ~ReferenceType() noexcept = default;

    /**
     * Increments the reference counter.
     */
    inline void AddRef[[= as::Behaviour(AS_NAMESPACE_QUALIFIER asBEHAVE_ADDREF)]]() const noexcept {
        AS_NAMESPACE_QUALIFIER asAtomicInc(m_referenceCounter);
    }

    /**
     * Decrements the reference counter, deleting this once the reference counter hits 0.
     */
    inline void Release[[= as::Behaviour(AS_NAMESPACE_QUALIFIER asBEHAVE_RELEASE)]]() const noexcept {
        if (AS_NAMESPACE_QUALIFIER asAtomicDec(m_referenceCounter) <= 0) { delete this; }
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
} // namespace as
