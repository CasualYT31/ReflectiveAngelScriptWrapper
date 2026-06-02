#include <AngelScriptWrapper/Engine.hpp>

namespace as {
int Engine::SetDefaultCallConvention(AS_NAMESPACE_QUALIFIER asDWORD callConv) noexcept {
    if (callConv != AS_NAMESPACE_QUALIFIER asCALL_CDECL && callConv != AS_NAMESPACE_QUALIFIER asCALL_STDCALL) {
        return AS_NAMESPACE_QUALIFIER asINVALID_ARG;
    } else {
        return m_defaultCallConvention = callConv;
    }
}

AS_NAMESPACE_QUALIFIER asDWORD Engine::applyDefaultCallConventionAsFallback(const int deducedCallConv) const {
    if (deducedCallConv >= 0) {
        return deducedCallConv;
    } else if (m_defaultCallConvention == AS_NAMESPACE_QUALIFIER asCALL_CDECL) {
        if (deducedCallConv == -2) {
            return AS_NAMESPACE_QUALIFIER asCALL_CDECL_OBJFIRST;
        } else if (deducedCallConv == -3) {
            return AS_NAMESPACE_QUALIFIER asCALL_CDECL_OBJLAST;
        }
    }
    return m_defaultCallConvention;
}
} // namespace as
