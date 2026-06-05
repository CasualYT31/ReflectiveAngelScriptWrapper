/**
 * @file ScriptDebugging.hpp
 * Contains helper code used to inspect the AngelScript environment during testing.
 */

#pragma once

#include <angelscript.h>
// TODO: do not like having to rebuild all the tests every time I make one change in one of the header files.
//       I should make Engine implement an interface, and then include the interface here instead.
#include <AngelScriptWrapper/Engine.hpp>
#include <iostream>

#define COMMA ,
#define STATIC_ASSERT_EQ(a, b) static_assert(a == b, a);

namespace as {
/**
 * Simple script message callback that pipes messages to stdout.
 * @param msg The script's message.
 */
inline void ScriptMessageCallback(AS_NAMESPACE_QUALIFIER asSMessageInfo* msg, void*) {
    std::cout << msg->message << '\n';
}

/**
 * Sets a script message callback on a script engine which can be used for debugging.
 * Every time the script engine encounters an error whilst compiling a script, a message will be written to stdout.
 * @param engine The engine to set the script message callback of.
 */
template <EngineOptions Opts> inline void SetMessageCallback(Engine<Opts>& engine) {
    engine.Ptr()->SetMessageCallback(
        AS_NAMESPACE_QUALIFIER asFUNCTION(ScriptMessageCallback), nullptr, AS_NAMESPACE_QUALIFIER asCALL_CDECL
    );
}
} // namespace as
