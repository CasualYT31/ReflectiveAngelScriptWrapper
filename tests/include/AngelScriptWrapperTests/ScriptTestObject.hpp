/**
 * @file ScriptTestObject.hpp
 * Defines a simple reference-counted type.
 */

#pragma once

#include <angelscript.h>
#include <AngelScriptWrapper/TypeDecl.hpp>

namespace as {
/**
 * A simple reference-counted type for testing.
 */
struct[[= RefType]] ScriptTestObject {
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

    /**
     * Register ScriptTestObject with an AngelScript engine.
     * @param engine The engine to register the type with.
     * @return The ID of the ScriptTestObject type, or a negative value on error.
     */
    static int Register(AS_NAMESPACE_QUALIFIER asIScriptEngine* const engine) {
        int typeId = 0;

        if (typeId = engine->RegisterObjectType(
                "ScriptTestObject",
                sizeof(ScriptTestObject),
                AS_NAMESPACE_QUALIFIER asOBJ_REF | AS_NAMESPACE_QUALIFIER asOBJ_NOCOUNT
            );
            typeId < 0) {
            return typeId;
        }

        if (const int r =
                engine->RegisterObjectProperty("ScriptTestObject", "int32 counter", asOFFSET(ScriptTestObject, counter));
            r < 0) {
            return r;
        }

        if (const int r = engine->RegisterObjectMethod(
                "ScriptTestObject",
                "int32 AddRef() const",
                AS_NAMESPACE_QUALIFIER asMETHOD(ScriptTestObject, AddRef),
                AS_NAMESPACE_QUALIFIER asCALL_THISCALL
            );
            r < 0) {
            return r;
        }

        if (const int r = engine->RegisterObjectMethod(
                "ScriptTestObject",
                "int32 Release() const",
                AS_NAMESPACE_QUALIFIER asMETHOD(ScriptTestObject, Release),
                AS_NAMESPACE_QUALIFIER asCALL_THISCALL
            );
            r < 0) {
            return r;
        }

        return typeId;
    }
};
} // namespace as
