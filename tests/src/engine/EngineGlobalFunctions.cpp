#include <AngelScriptWrapper/Engine.hpp>
#include <AngelScriptWrapperTests/ScriptDebugging.hpp>
#include <gtest/gtest.h>
#include <scripthelper.h>

inline int add(const int a, const int b) {
    return a + b;
}

inline int add2(const int a, const int b) {
    return a + b;
}

inline double add2(const double a, const double b) {
    return a - b;
}

struct Global {
    inline int add(const int a, const int b) {
        return a + b;
    }

    inline const double add2(const double a, const double b) {
        return a + b;
    }

    inline const double add2(const double a, const double b) const {
        return a - b;
    }
};

TEST(AngelScriptEngineGlobalFunctions, SimpleFunction) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());

    ASSERT_GE(engine.RegisterGlobalFunction<^^add>(), 0);

    int result = 0;
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return add(5, 6);", &result, 4), 0);

    EXPECT_EQ(result, 11);
}

TEST(AngelScriptEngineGlobalFunctions, SimpleMethod) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());
    as::SetMessageCallback(engine);
    Global obj;

    ASSERT_GE(engine.RegisterGlobalFunction<^^Global::add>(&obj), 0);

    int result = 0;
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return add(5, 6);", &result, 4), 0);

    EXPECT_EQ(result, 11);
}

TEST(AngelScriptEngineGlobalFunctions, SimpleFunctionOverload) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());

    constexpr auto intendedAdd = as::FindOverload<^^::, double, false, const double, const double>("add2");
    ASSERT_GE(engine.RegisterGlobalFunction<intendedAdd>(), 0);

    double result = 0;
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return add2(5.5, 2.5);", &result, 11), 0);

    EXPECT_DOUBLE_EQ(result, 3.0);
}

TEST(AngelScriptEngineGlobalFunctions, SimpleMethodOverload) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());
    as::SetMessageCallback(engine);
    Global obj;

    constexpr auto intendedAdd = as::FindOverload<^^Global, const double, true, const double, const double>("add2");
    ASSERT_GE(engine.RegisterGlobalFunction<intendedAdd>(&obj), 0);

    double result = 0;
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return add2(5.5, 2.5);", &result, 11), 0);

    EXPECT_DOUBLE_EQ(result, 3.0);
}

// TODO: how to test call convention is being set correctly?
//       Using stdcall instead of cdecl or vice versa won't necessarily crash the test program.
//       And even if it did, I don't want to rely on UB.
//       I don't want to add an overload that out&s the call convention used, either, but my hand might be forced.
