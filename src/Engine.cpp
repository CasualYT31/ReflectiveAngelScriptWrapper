#include <AngelScriptWrapper/Engine.hpp>
#include <AngelScriptWrapper/OwnedObject.hpp>
#include <AngelScriptWrapper/SharedObject.hpp>

namespace as {
Engine::Engine() {
    m_engine = std::make_unique<OwnedObject<::asIScriptEngine>>(asCreateScriptEngine());
}

Engine::Engine(::asIScriptEngine* const engine) {
    m_engine = std::make_unique<SharedObject<::asIScriptEngine>>(engine);
}
} // namespace as
