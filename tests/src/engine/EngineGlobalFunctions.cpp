#include <AngelScriptWrapper/Engine.hpp>
#include <gtest/gtest.h>
#include <scripthelper.h>

int add(const int a, const int b) {
    return a + b;
}

TEST(AngelScriptEngineGlobalFunctions, SimpleFunction) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());

    ASSERT_GE(engine.RegisterGlobalFunction<^^add>(), 0);

    int result = 0;
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return add(5, 6);", &result, 4), 0);

    EXPECT_EQ(result, 11);
}
