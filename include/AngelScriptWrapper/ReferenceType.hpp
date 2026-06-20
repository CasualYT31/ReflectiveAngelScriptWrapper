/**
 * @file ReferenceType.hpp
 * Defines a base class you can inherit from that provides reference counting for you.
 */

#pragma once

#include <angelscript.h>

namespace as {
/**
 * Base class containing standard reference counting logic.
 * This class makes more sense as a mixin, as otherwise every one of your reference types will have individual
 * ReferenceType base classes, which is not useful in a scripting context.
 * @tparam T Subclasses need to provide their type [all the way] up to ReferenceType.
 */
template <typename T> struct[[= as::Mixin]] ReferenceType {
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
} // namespace as
