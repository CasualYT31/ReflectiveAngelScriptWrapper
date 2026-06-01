#include <AngelScriptWrapper/Engine.hpp>

namespace as {
int Engine::SetDefaultCallingConvention(AS_NAMESPACE_QUALIFIER asDWORD callConv) noexcept {
    if (callConv != AS_NAMESPACE_QUALIFIER asCALL_CDECL && callConv != AS_NAMESPACE_QUALIFIER asCALL_STDCALL) {
        return AS_NAMESPACE_QUALIFIER asINVALID_ARG;
    } else {
        return m_defaultCallingConvention = callConv;
    }
}
} // namespace as
