/**
 * @file OwnedObject.hpp
 * A wrapper for a generic AngelScript object that takes ownership of the object it is given.
 */

#pragma once

#include <AngelScriptWrapper/IsReferenceCounted.hpp>
#include <utility>

namespace as {
/**
 * A wrapper around an AngelScript object that manages its reference counter for you.
 */
template <IsReferenceCounted T> struct OwnedObject {
    /**
     * Initializes the wrapper with no object.
     */
    OwnedObject() = default;

    /**
     * Initializes the wrapper with an existing AngelScript object.
     * The wrapper assumes ownership of the object, and so it will NOT increment the object's reference counter.
     * @param o The object to hold a reference of.
     */
    inline OwnedObject(T* const o) : m_ptr(o) {}

    /**
     * You can't copy an OwnedObject wrapper.
     * If you need to copy this wrapper object, you must copy it into an Object wrapper instead.
     */
    OwnedObject(OwnedObject<T> const&) = delete;

    /**
     * Moves the AngelScript object pointer and increments its reference counter.
     * Even though we're "moving," we are technically creating another reference to the object, so we need to increment
     * the reference counter anyway. If you remove this behavior, \c emplace_back() and other "move" calls will cause
     * the reference counter to fall down by one, which will cause crashes in your program.
     * @param w The wrapper object to move.
     */
    inline OwnedObject(OwnedObject<T>&& w) noexcept : m_ptr(std::move(w.m_ptr)) {
        if (m_ptr) { m_ptr->AddRef(); }
    }

    /**
     * Releases this wrapper's current pointer, then assigns a new object to the wrapper.
     * The reference counter of o is NOT incremented as the wrapper assumes ownership of it.
     * @param o The object pointer to assign to this wrapper.
     * @return This wrapper object.
     */
    inline OwnedObject<T>& operator=(T* const o) {
        if (m_ptr) { m_ptr->Release(); }
        m_ptr = o;
        return *this;
    }

    /**
     * You cannot copy an OwnedObject wrapper into another one.
     * If you wish to copy an OwnedObject, you must copy it into an Object instead.
     */
    OwnedObject<T>& operator=(OwnedObject<T> const&) = delete;

    /**
     * Releases this wrapper's current pointer, then moves another pointer into this wrapper.
     * @param w The wrapper object to move into this one.
     * @return This wrapper object.
     */
    inline OwnedObject<T>& operator=(OwnedObject<T>&& w) noexcept {
        if (m_ptr) { m_ptr->Release(); }
        m_ptr = std::move(w.m_ptr);
        // We're technically creating a new reference to the object despite the move semantics.
        if (m_ptr) { m_ptr->AddRef(); }
        return *this;
    }

    /**
     * Releases the reference to the AngelScript object.
     */
    inline ~OwnedObject() noexcept {
        if (m_ptr) { m_ptr->Release(); }
    }

    /**
     * Access the AngelScript object that is managed by this wrapper.
     * @return The underlying AngelScript object.
     */
    inline T& operator*() const {
        return *m_ptr;
    }

    /**
     * Access the raw pointer to the AngelScript object that is managed by this wrapper.
     * @return The underlying AngelScript object's pointer.
     */
    inline T* operator->() const {
        return m_ptr;
    }

    /**
     * Finds out if a pointer to an AngelScript object is stored in this wrapper.
     * @return True if this wrapper holds a pointer to an object, false if not.
     */
    inline operator bool() const {
        return m_ptr;
    }

private:
    /**
     * The AngelScript object to manage the reference counter of.
     */
    T* m_ptr = nullptr;
};
} // namespace as
