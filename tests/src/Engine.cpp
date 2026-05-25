#include <angelscript.h>
#include <AngelScriptWrapper/Engine.hpp>
#include <gtest/gtest.h>

TEST(AngelScriptEngine, DefaultConstructor) {
    as::Engine engine;
    ASSERT_TRUE(engine.engine());
    ASSERT_EQ(engine.engine()->AddRef(), 2);
    ASSERT_EQ(engine.engine()->Release(), 1);
}

TEST(AngelScriptEngine, PointerConstructor) {
    const auto pEngine = ::asCreateScriptEngine();
    ASSERT_TRUE(pEngine);

    {
        as::Engine engine(pEngine);
        ASSERT_EQ(engine.engine()->AddRef(), 3);
        ASSERT_EQ(engine.engine()->Release(), 2);
    }

    ASSERT_EQ(pEngine->Release(), 0);
}
