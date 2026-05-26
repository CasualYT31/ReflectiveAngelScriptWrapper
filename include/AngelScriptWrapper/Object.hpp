/**
 * @file Object.hpp
 * Defines the interface for object wrapper-based classes.
 */

#pragma once

#include <AngelScriptWrapper/Concepts.hpp>

namespace as {
/**
 * Interface that all AngelScript object wrapper classes must implement.
 * @tparam T The type of AngelScript object to store.
 */
template <IsReferenceCounted T> struct Object {
    virtual ~Object() = default;

    /**
     * An object wrapper must grant access to the underlying AngelScript object.
     * @return The AngelScript object.
     */
    virtual T& operator*() const = 0;

    /**
     * An object wrapper must grant access to the underlying AngelScript object's members.
     * @return The AngelScript object's raw pointer.
     */
    virtual T* operator->() const = 0;

    /**
     * An object wrapper must evaluate to true if it stores an object, and false if it does not.
     * @return False if the wrapper holds nullptr, true otherwise.
     */
    virtual operator bool() const = 0;
};
} // namespace as
