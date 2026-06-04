/**
 * @file TestRefType.hpp
 * Defines a reference type used throughout the unit tests.
 */

#pragma once

#include <AngelScriptWrapper/ReferenceType.hpp>
#include <AngelScriptWrapper/TypeDecl.hpp>

namespace as {
struct[[= as::RefType]] TestRefType : public ReferenceTypeWithDefaultFactory<TestRefType> {
    /**
     * Register TestRefType with an unwrapped AngelScript engine.
     * @param engine The engine to register the type with.
     * @return The ID of the TestRefType type, or a negative value on error.
     */
    static int Register(AS_NAMESPACE_QUALIFIER asIScriptEngine* const engine) {
        int typeId = 0;

        if (typeId = engine->RegisterObjectType("TestRefType", 0, AS_NAMESPACE_QUALIFIER asOBJ_REF); typeId < 0) {
            return typeId;
        }

        if (const int r = engine->RegisterObjectBehaviour(
                "TestRefType",
                AS_NAMESPACE_QUALIFIER asBEHAVE_ADDREF,
                "void f()",
                AS_NAMESPACE_QUALIFIER asMETHOD(TestRefType, AddRef),
                AS_NAMESPACE_QUALIFIER asCALL_THISCALL
            );
            r < 0) {
            return r;
        }

        if (const int r = engine->RegisterObjectBehaviour(
                "TestRefType",
                AS_NAMESPACE_QUALIFIER asBEHAVE_RELEASE,
                "void f()",
                AS_NAMESPACE_QUALIFIER asMETHOD(TestRefType, Release),
                AS_NAMESPACE_QUALIFIER asCALL_THISCALL
            );
            r < 0) {
            return r;
        }

        if (const int r = engine->RegisterObjectBehaviour(
                "TestRefType",
                AS_NAMESPACE_QUALIFIER asBEHAVE_FACTORY,
                "TestRefType@ f()",
                AS_NAMESPACE_QUALIFIER asFUNCTION(TestRefType::Factory),
                AS_NAMESPACE_QUALIFIER asCALL_CDECL
            );
            r < 0) {
            return r;
        }

        if (const int r = engine->RegisterObjectMethod(
                "TestRefType",
                "int32 RefCount() const",
                AS_NAMESPACE_QUALIFIER asMETHOD(TestRefType, RefCount),
                AS_NAMESPACE_QUALIFIER asCALL_THISCALL
            );
            r < 0) {
            return r;
        }

        return typeId;
    }
};
} // namespace as
