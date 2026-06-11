/**
 * @file ScriptDebugging.hpp
 * Contains helper code used to inspect the AngelScript environment during testing.
 */

#pragma once

#include <angelscript.h>
#include <iostream>
#include <string>
#include <vector>

namespace as {
/**
 * Simple script message callback that pipes messages to stdout.
 * @param msg The script's message.
 */
inline void ScriptMessageCallback(AS_NAMESPACE_QUALIFIER asSMessageInfo* msg, void*) {
    std::cout << msg->message << '\n';
}

/**
 * Version of the script message callback that pushes messages to a string vector.
 * @param msg The script's message.
 * @param state The vector to push messages to.
 */
inline void ScriptMessageCallback(AS_NAMESPACE_QUALIFIER asSMessageInfo* msg, std::vector<std::string>* state) {
    // std::cout << msg->message << '\n';
    state->emplace_back(msg->message);
}

/**
 * Concept used to prevent including the entire Engine.hpp header to support the SetMessageCallback() function.
 * @tparam T The type to test.
 */
template <typename T>
concept IsEngineWrapper = requires(T t) { t.Ptr(); };

/**
 * Sets a script message callback on a script engine that can be used for debugging.
 * Every time the script engine encounters an error whilst compiling a script, a message will be written to stdout.
 * @tparam T The type of engine to set the script message callback of.
 * @param engine The engine to set the script message callback of.
 * @return The result of the message callback registration.
 */
template <typename T>
    requires(IsEngineWrapper<T>)
inline int SetMessageCallback(T& engine) {
    return engine.Ptr()->SetMessageCallback(
        AS_NAMESPACE_QUALIFIER asFUNCTIONPR(
            ScriptMessageCallback, (AS_NAMESPACE_QUALIFIER asSMessageInfo*, void*), void
        ),
        nullptr,
        AS_NAMESPACE_QUALIFIER asCALL_CDECL
    );
}

/**
 * Sets a script message callback on a script engine that can be used to gather engine messages for testing purposes.
 * Every time the script engine encounters an error whilst compiling a script, a message will be written to the given
 * list.
 * @tparam T The type of engine to set the script message callback of.
 * @param engine The engine to set the script message callback of.
 * @param state Pointer to a message list that will be appended to whenever the engine encounters an error.
 * @return The result of the message callback registration.
 */
template <typename T>
    requires(IsEngineWrapper<T>)
inline int SetMessageCallback(T& engine, std::vector<std::string>* state) {
    return engine.Ptr()->SetMessageCallback(
        AS_NAMESPACE_QUALIFIER asFUNCTIONPR(
            ScriptMessageCallback, (AS_NAMESPACE_QUALIFIER asSMessageInfo*, std::vector<std::string>*), void
        ),
        state,
        AS_NAMESPACE_QUALIFIER asCALL_CDECL
    );
}
} // namespace as
