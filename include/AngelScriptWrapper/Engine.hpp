/**
 * @file Engine.hpp
 * Defines the wrapper class for an asIScriptEngine instance.
 */

#pragma once

#include <angelscript.h>
#include <AngelScriptWrapper/Object.hpp>
#include <memory>

namespace as {
/**
 * A wrapper around an asIScriptEngine instance.
 */
struct Engine {
    /**
     * Initializes a new asIScriptEngine instance inside a wrapper object.
     */
    Engine();

    /**
     * Initializes an engine wrapper with an existing asIScriptEngine instance.
     * @warning Usually this wrapper object will release the engine when the wrapper object is destructed, but if the
     *          wrapper is constructed in this way, then the engine will not be released and it is the responsibility of
     *          the caller to release the engine that it provides here.
     */
    Engine(::asIScriptEngine* const engine);

    /**
     * Grants access to the underlying scripting engine instance.
     * @warning If this wrapper owns the returned engine instance, then the returned pointer will become invalid once
     *          the wrapper is destructed.
     * @return Pointer to the scripting engine.
     */
    inline ::asIScriptEngine* engine() const noexcept {
        return &**m_engine;
    }

private:
    /**
     * Points to the AngelScript engine instance this wrapper interfaces with.
     * Becomes either Owned or Shared depending on how the Engine was constructed.
     */
    std::unique_ptr<Object<::asIScriptEngine>> m_engine;
};
} // namespace as
