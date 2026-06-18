#include <angelscript.h>
#include <AngelScriptWrapper/Engine.hpp>
#include <gtest/gtest.h>

static int randomVariable = 0;

TEST(AngelScriptEngineAuxiliaryMap, Setter) {
    as::Engine engine;
    EXPECT_EQ(engine.GetAuxiliaryObjectCount(), 0);

    const as::AuxiliaryMap TestMap = {
        { "Label1", nullptr },
        { "Label2", &randomVariable },
    };
    engine.SetAuxiliaryMap(TestMap);
    EXPECT_EQ(engine.GetAuxiliaryObjectCount(), TestMap.size());

    for (const auto& kv : TestMap) { EXPECT_EQ(engine.GetAuxiliaryObject(kv.first), kv.second); }
}

TEST(AngelScriptEngineAuxiliaryMap, AddSingle) {
    as::Engine engine;
    EXPECT_EQ(engine.GetAuxiliaryObjectCount(), 0);

    engine.AddAuxiliaryObject("MyLabel", &randomVariable);

    EXPECT_EQ(engine.GetAuxiliaryObjectCount(), 1);
    EXPECT_EQ(engine.GetAuxiliaryObject("MyLabel"), &randomVariable);
}

TEST(AngelScriptEngineAuxiliaryMap, AddSingleViaMultiple) {
    as::Engine engine;
    EXPECT_EQ(engine.GetAuxiliaryObjectCount(), 0);

    engine.AddAuxiliaryObjects("MyLabel", &randomVariable);

    EXPECT_EQ(engine.GetAuxiliaryObjectCount(), 1);
    EXPECT_EQ(engine.GetAuxiliaryObject("MyLabel"), &randomVariable);
}

TEST(AngelScriptEngineAuxiliaryMap, AddMultiple) {
    as::Engine engine;
    EXPECT_EQ(engine.GetAuxiliaryObjectCount(), 0);

    engine.AddAuxiliaryObjects("MyLabel", &randomVariable, "MySecondLabel", nullptr);

    EXPECT_EQ(engine.GetAuxiliaryObjectCount(), 2);
    EXPECT_EQ(engine.GetAuxiliaryObject("MyLabel"), &randomVariable);
    EXPECT_EQ(engine.GetAuxiliaryObject("MySecondLabel"), nullptr);
}

TEST(AngelScriptEngineAuxiliaryMap, AttemptToAddMultipleObjectsWithSameKey) {
    as::Engine engine;
    EXPECT_EQ(engine.GetAuxiliaryObjectCount(), 0);

    EXPECT_DEATH(
        (engine.AddAuxiliaryObjects("MyLabel", &randomVariable, "MyLabel", &randomVariable)),
        "auxiliary map already has an entry for this key"
    );
}
