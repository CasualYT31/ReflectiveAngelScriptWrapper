/**
 * @file ScriptTestObject.hpp
 * Defines a simple reference-counted type.
 */

#pragma once

namespace as {
/**
 * A simple reference-counted type for testing.
 */
struct ScriptTestObject {
    mutable int counter = 0;

    inline ScriptTestObject() {
        AddRef();
    }

    inline ~ScriptTestObject() {
        Release();
    }

    inline int AddRef() const {
        return ++counter;
    }

    inline int Release() const {
        return --counter;
    }
};
} // namespace as
