#include <AngelScriptWrapper/Engine.hpp>
#include <AngelScriptWrapperTests/ScriptDebugging.hpp>
#include <gtest/gtest.h>
#include <scriptarray.h>
#include <scripthelper.h>

std::uint64_t getSize(const AS_NAMESPACE_QUALIFIER CScriptArray* const arr[[= as::subtype::Int32]]);

AS_NAMESPACE_QUALIFIER asIScriptFunction* getFunc[[= as::Funcdef(^^getSize)]](
    asIScriptFunction* const handle[[= as::Funcdef(^^getSize)]]
) {
    return handle;
}

TEST(AngelScriptEngineFuncdefs, RegistrationAndTypenameGeneration) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());
    as::SetMessageCallback(engine);
    AS_NAMESPACE_QUALIFIER RegisterScriptArray(engine.Ptr(), false);

    ASSERT_GE(engine.RegisterFuncdef<^^getSize>(), 0);
    ASSERT_GE(engine.RegisterGlobalFunction<^^getFunc>(), 0);

    AS_NAMESPACE_QUALIFIER asIScriptModule* mod = engine.Ptr()->GetModule("test", AS_NAMESPACE_QUALIFIER asGM_ALWAYS_CREATE);
    ASSERT_TRUE(mod);

    // This script tests two things:
    // 1. that we can register functions in the application interface that accept and return function handles, and
    // 2. that the funcdef type can be used within the scripts.
    ASSERT_GE(
        mod->AddScriptSection(
            "test.as",
            "uint64 sizeGetter(const array<int>@ arr) { return arr.length(); }"
            "getSize@ main() { return getFunc(sizeGetter); }"
        ),
        0
    );

    ASSERT_GE(mod->Build(), 0);

    AS_NAMESPACE_QUALIFIER asIScriptContext* ctx = engine.Ptr()->CreateContext();
    ASSERT_TRUE(ctx);

    AS_NAMESPACE_QUALIFIER asIScriptFunction* func = mod->GetFunctionByDecl("getSize@ main()");
    ASSERT_TRUE(func);

    ASSERT_GE(ctx->Prepare(func), 0);

    ASSERT_EQ(ctx->Execute(), AS_NAMESPACE_QUALIFIER asEXECUTION_FINISHED);

    AS_NAMESPACE_QUALIFIER asIScriptFunction* expected = mod->GetFunctionByDecl("uint64 sizeGetter(const array<int>@)");
    ASSERT_TRUE(expected);

    AS_NAMESPACE_QUALIFIER asIScriptFunction* res =
        static_cast<AS_NAMESPACE_QUALIFIER asIScriptFunction*>(ctx->GetReturnObject());
    EXPECT_EQ(res, expected);
}
