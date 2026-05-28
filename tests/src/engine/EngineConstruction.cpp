#include <angelscript.h>
#include <AngelScriptWrapper/Engine.hpp>
#include <gtest/gtest.h>

TEST(AngelScriptEngineConstruction, DefaultConstructor) {
    as::Engine engine;
    ASSERT_TRUE(engine.Ptr());
    ASSERT_EQ(engine.Ptr()->AddRef(), 2);
    ASSERT_EQ(engine.Ptr()->Release(), 1);
    EXPECT_TRUE(engine.HasEngine());
}

TEST(AngelScriptEngineConstruction, PointerConstructor) {
    const auto pEngine = AS_NAMESPACE_QUALIFIER asCreateScriptEngine();
    ASSERT_TRUE(pEngine);

    {
        as::Engine engine(pEngine);
        ASSERT_EQ(engine.Ptr()->AddRef(), 3);
        ASSERT_EQ(engine.Ptr()->Release(), 2);
        EXPECT_TRUE(engine.HasEngine());
    }

    ASSERT_EQ(pEngine->Release(), 0);
}
