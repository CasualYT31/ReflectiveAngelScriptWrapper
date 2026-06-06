#include <AngelScriptWrapper/Engine.hpp>
#include <AngelScriptWrapperTests/ScriptDebugging.hpp>
#include <gtest/gtest.h>
#include <scripthelper.h>

enum class MyEnum {
    First,
    Second[[= as::Name("Two")]],
    Third,
};

TEST(AngelScriptEngineEnums, Registration) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());
    as::SetMessageCallback(engine);

    const auto typeId = engine.RegisterEnum<^^MyEnum>();
    ASSERT_GE(typeId, 0);

    MyEnum res = MyEnum::Third;
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return MyEnum::Two;", &res, typeId), 0);
    EXPECT_EQ(res, MyEnum::Second);
    EXPECT_EQ(static_cast<int>(res), 1);
}

enum[[= as::Name("NormalEnum")]] CastedEnum : std::uint8_t{
    One,
    Two,
    Three,
    Four = 60,
};

TEST(AngelScriptEngineEnums, RegistrationWithCastingToInt) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());
    as::SetMessageCallback(engine);

    const auto typeId = engine.RegisterEnum<^^CastedEnum>();
    ASSERT_GE(typeId, 0);

    CastedEnum res = CastedEnum::Two;
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return NormalEnum::Four;", &res, typeId), 0);
    EXPECT_EQ(res, CastedEnum::Four);
    EXPECT_EQ(res, 60);
}
