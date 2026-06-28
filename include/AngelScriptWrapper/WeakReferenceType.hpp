/**
 * @file WeakReferenceType.hpp
 * Defines a base class you can inherit from that provides reference counting and weakref behavior for you.
 */

#pragma once

#include <angelscript.h>
#include <AngelScriptWrapper/OwnedObject.hpp>
#include <AngelScriptWrapper/ReferenceType.hpp>

namespace as {
/**
 * Base class containing standard reference counting logic alongside weakref logic.
 * Implementation based on https://www.angelcode.com/angelscript/sdk/docs/manual/doc_adv_weakref.html.
 */
struct[[= as::Mixin]] WeakReferenceType : public ReferenceType {
    /**
     * Copies of a weak reference type will have their reference counter start at 1, and their weak reference flag
     * uninitialized.
     * Explicitly writing the copy constructor like this prevents the default one from copying the reference counter and
     * the weak reference flag from the given object.
     */
    inline WeakReferenceType(WeakReferenceType const&) {}

    /**
     * Move construction has the same logic as copy construction.
     */
    inline WeakReferenceType(WeakReferenceType&&) {}

    /**
     * Don't forget that if you intend to introduce a base class of your own that inherits from WeakReferenceType, you
     * @b must make its destructor virtual.
     */
    virtual inline ~WeakReferenceType() noexcept = default;

    /**
     * Think of copy assignment as replacing the contents of the object with another.
     * We should copy everything EXCEPT the reference counter and weak reference flag, since the object given, and this
     * object, are separate.
     */
    inline WeakReferenceType& operator=(WeakReferenceType const&) {
        return *this;
    }

    /**
     * Move assignment has the same logic as copy assignment.
     * This operator must not be responsible for decreasing the reference counter of the given object, since it
     * technically still has a reference held on it after the call. Let it decrease naturally via RAII.
     */
    inline WeakReferenceType& operator=(WeakReferenceType&&) {
        return *this;
    }

    /**
     * Decrements the reference counter, deleting this once the reference counter hits 0.
     * Also sets the weak reference flag to true is the reference counter hits 0.
     */
    virtual inline void Release[[= as::Behaviour(AS_NAMESPACE_QUALIFIER asBEHAVE_RELEASE)]]() const noexcept {
        if (GetRefCount() == 1 && m_weakReferenceFlag) { m_weakReferenceFlag->Set(true); }
        ReferenceType::Release();
    }

    /**
     * Used by the scripting engine to access this object's weak reference flag.
     * It will create one if one hasn't been created yet.
     * @return The pointer to the weak reference flag.
     */
    inline AS_NAMESPACE_QUALIFIER asILockableSharedBool* GetWeakRefFlag[
        [= as::Behaviour(AS_NAMESPACE_QUALIFIER asBEHAVE_GET_WEAKREF_FLAG)]]() const noexcept {
        if (!m_weakReferenceFlag) {
            AS_NAMESPACE_QUALIFIER asAcquireExclusiveLock();
            if (!m_weakReferenceFlag) { m_weakReferenceFlag = AS_NAMESPACE_QUALIFIER asCreateLockableSharedBool(); }
            AS_NAMESPACE_QUALIFIER asReleaseExclusiveLock();
        }
        return m_weakReferenceFlag.Ptr();
    }

protected:
    /**
     * WeakReferenceType can only be inherited from.
     */
    WeakReferenceType() = default;

private:
    /**
     * The weak reference flag used to track if the object has been destroyed.
     * The OwnedObject destructor will handle releasing the weak reference flag for us.
     */
    mutable OwnedObject<AS_NAMESPACE_QUALIFIER asILockableSharedBool> m_weakReferenceFlag;
};
} // namespace as
