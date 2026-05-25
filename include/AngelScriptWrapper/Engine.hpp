/**
 * @file Engine.hpp
 * Defines the wrapper class for an asIScriptEngine instance.
 */

#pragma once

#include "Object.hpp"

#include <angelscript.h>

namespace as {
// /**
//  * A wrapper around an asIScriptEngine instance.
//  */
// class AngelScriptEngine {
// public:
//     /**
//      * Initializes a new asIScriptEngine instance inside a wrapper object.
//      */
//     AngelScriptEngine();

// /**
//  * Initializes an engine wrapper with an existing asIScriptEngine instance.
//  * @warning Usually this wrapper object will release the engine in its destructor, but if the wrapper is constructed
//  *          in this way, then the engine will not be released and it is the responsibility of the caller to release
//  *          the engine that it provides here.
//  */
// AngelScriptEngine(::asIScriptEngine* const engine) noexcept : m_engine(engine), m_ownsEngine(false) {}

// AngelScriptEngine(AngelScriptEngine const& wrapper) {}

// /**
//  * Move constructor for an AngelScript engine wrapper.
//  * @param wrapper The wrapper object to move.
//  */
// AngelScriptEngine(AngelScriptEngine&& wrapper) noexcept
//     : m_engine(std::move(wrapper.m_engine)),
//       m_ownsEngine(std::move(wrapper.m_ownsEngine)) {}

// /**
//  * Destroys the asIScriptEngine instance, but only if it was created by this wrapper.
//  */
// ~AngelScriptEngine() noexcept;

// /**
//  * Grants access to the underlying scripting engine instance.
//  * @warning If this wrapper owns the returned engine instance, then the returned pointer will become invalid once
//  *          the wrapper is destructed.
//  * @return Pointer to the scripting engine.
//  */
// inline ::asIScriptEngine* engine() const noexcept {
//     return m_engine;
// }

// private:
//     /**
//      * Points to the AngelScript engine instance this wrapper interfaces with.
//      */
//     Object<::asIScriptEngine> m_engine;
// };
} // namespace as
