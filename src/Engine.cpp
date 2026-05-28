#include <AngelScriptWrapper/Engine.hpp>
#include <AngelScriptWrapper/OwnedObject.hpp>
#include <AngelScriptWrapper/SharedObject.hpp>

namespace as {
Engine::Engine() {
    m_engine =
        std::make_unique<OwnedObject<AS_NAMESPACE_QUALIFIER asIScriptEngine>>(AS_NAMESPACE_QUALIFIER asCreateScriptEngine());
}

Engine::Engine(AS_NAMESPACE_QUALIFIER asIScriptEngine* const engine) {
    m_engine = std::make_unique<SharedObject<AS_NAMESPACE_QUALIFIER asIScriptEngine>>(engine);
}
} // namespace as
