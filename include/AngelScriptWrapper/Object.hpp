/**
 * @file Object.hpp
 * A wrapper for a generic AngelScript object.
 */

#pragma once

#include <AngelScriptWrapper/IsReferenceCounted.hpp>
#include <AngelScriptWrapper/OwnedObject.hpp>
#include <utility>

namespace as {
/**
 * A wrapper around an AngelScript object that manages its reference counter for you.
 */
template <IsReferenceCounted T> struct Object {
    /**
     * Initializes the wrapper with no object.
     */
    Object() = default;

    /**
     * Initializes the wrapper with an existing AngelScript object.
     * Since the pointer is copied into this wrapper, the wrapper does not assume ownership of the object, and so it
     * will increment the object's reference counter.
     * @param o The object to hold a reference of.
     */
    inline Object(T* const o) : m_ptr(o) {
        if (m_ptr) { m_ptr->AddRef(); }
    }

    /**
     * Copies the AngelScript object pointer and increments its reference counter.
     * @param w The wrapper object to copy.
     */
    inline Object(Object<T> const& w) : m_ptr(w.m_ptr) {
        if (m_ptr) { m_ptr->AddRef(); }
    }

    /**
     * Copies an AngelScript object pointer from an owning wrapper and increments its reference counter.
     * @param w The wrapper object to copy.
     */
    inline Object(OwnedObject<T> const& w) : m_ptr(&*w) {
        if (m_ptr) { m_ptr->AddRef(); }
    }

    /**
     * Moves the AngelScript object pointer and increments its reference counter.
     * Even though we're "moving," we are technically creating another reference to the object, so we need to increment
     * the reference counter anyway. If you remove this behavior, \c emplace_back() and other "move" calls will cause
     * the reference counter to fall down by one, which will cause crashes in your program.
     * @param w The wrapper object to move.
     */
    inline Object(Object<T>&& w) noexcept : m_ptr(std::move(w.m_ptr)) {
        if (m_ptr) { m_ptr->AddRef(); }
    }

    /**
     * Releases this wrapper's current pointer, then assigns a new object to the wrapper, before incrementing its
     * reference counter.
     * The reference counter of o is incremented as the wrapper does not assume ownership of it.
     * @param o The object pointer to assign to this wrapper.
     * @return This wrapper object.
     */
    inline Object<T>& operator=(T* const o) {
        if (m_ptr) { m_ptr->Release(); }
        m_ptr = o;
        if (m_ptr) { m_ptr->AddRef(); }
        return *this;
    }

    /**
     * Releases this wrapper's current pointer, then copies another pointer into this wrapper.
     * @param w The wrapper object to copy into this one.
     * @return This wrapper object.
     */
    inline Object<T>& operator=(Object<T> const& w) {
        if (m_ptr) { m_ptr->Release(); }
        m_ptr = w.m_ptr;
        if (m_ptr) { m_ptr->AddRef(); }
        return *this;
    }

    /**
     * Releases this wrapper's current pointer, then copies another pointer into this wrapper.
     * @param w The wrapper object to copy into this one.
     * @return This wrapper object.
     */
    inline Object<T>& operator=(OwnedObject<T> const& w) {
        if (m_ptr) { m_ptr->Release(); }
        m_ptr = &*w;
        if (m_ptr) { m_ptr->AddRef(); }
        return *this;
    }

    /**
     * Releases this wrapper's current pointer, then moves another pointer into this wrapper.
     * @param w The wrapper object to move into this one.
     * @return This wrapper object.
     */
    inline Object<T>& operator=(Object<T>&& w) noexcept {
        if (m_ptr) { m_ptr->Release(); }
        m_ptr = std::move(w.m_ptr);
        if (m_ptr) { m_ptr->AddRef(); }
        return *this;
    }

    /**
     * Releases the reference to the AngelScript object.
     */
    inline ~Object() noexcept {
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
