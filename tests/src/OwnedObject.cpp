#include <AngelScriptWrapper/OwnedObject.hpp>
#include <AngelScriptWrapperTests/ScriptTestObject.hpp>
#include <gtest/gtest.h>

TEST(AngelScriptOwnedObject, DefaultConstructor) {
    as::OwnedObject<as::ScriptTestObject> object;
    EXPECT_FALSE(object);
    EXPECT_FALSE(object.operator->());
}

TEST(AngelScriptOwnedObject, PointerConstructor) {
    as::ScriptTestObject counter;
    EXPECT_EQ(counter.counter, 1);

    {
        as::OwnedObject object(&counter);
        ASSERT_TRUE(object);
        ASSERT_EQ(object.operator->(), &counter);
        EXPECT_EQ(counter.counter, 1);
        EXPECT_EQ(object->counter, 1);
        EXPECT_EQ((*object).counter, 1);
    }

    EXPECT_EQ(counter.counter, 0);
}

TEST(AngelScriptOwnedObject, MoveConstructor) {
    as::ScriptTestObject counter;
    EXPECT_EQ(counter.counter, 1);

    {
        as::OwnedObject original(&counter);
        ASSERT_TRUE(original);
        ASSERT_EQ(original.operator->(), &counter);
        EXPECT_EQ(counter.counter, 1);
        EXPECT_EQ(original->counter, 1);
        EXPECT_EQ((*original).counter, 1);

        {
            as::OwnedObject moved(std::move(original));
            ASSERT_TRUE(moved);
            ASSERT_EQ(moved.operator->(), &counter);
            ASSERT_EQ(moved.operator->(), original.operator->());
            EXPECT_EQ(counter.counter, 2);
            EXPECT_EQ(moved->counter, 2);
            EXPECT_EQ((*moved).counter, 2);
            EXPECT_EQ(original->counter, 2);
            EXPECT_EQ((*original).counter, 2);
        }

        ASSERT_TRUE(original);
        ASSERT_EQ(original.operator->(), &counter);
        EXPECT_EQ(counter.counter, 1);
        EXPECT_EQ(original->counter, 1);
        EXPECT_EQ((*original).counter, 1);
    }

    EXPECT_EQ(counter.counter, 0);
}

TEST(AngelScriptOwnedObject, PointerAssignment) {
    as::ScriptTestObject counter1, counter2;
    EXPECT_EQ(counter1.counter, 1);
    EXPECT_EQ(counter2.counter, 1);

    {
        as::OwnedObject object(&counter1);
        ASSERT_TRUE(object);
        ASSERT_EQ(object.operator->(), &counter1);
        EXPECT_EQ(counter1.counter, 1);
        EXPECT_EQ(counter2.counter, 1);
        EXPECT_EQ(object->counter, 1);
        EXPECT_EQ((*object).counter, 1);

        ASSERT_TRUE((object = &counter2).operator bool());
        ASSERT_EQ(object.operator->(), &counter2);
        EXPECT_EQ(counter1.counter, 0);
        EXPECT_EQ(counter2.counter, 1);
        EXPECT_EQ(object->counter, 1);
        EXPECT_EQ((*object).counter, 1);

        EXPECT_FALSE((object = nullptr).operator bool());
        EXPECT_EQ(object.operator->(), nullptr);
        EXPECT_EQ(counter1.counter, 0);
        EXPECT_EQ(counter2.counter, 0);

        ASSERT_TRUE((object = &counter1).operator bool());
        ASSERT_EQ(object.operator->(), &counter1);
        EXPECT_EQ(counter1.counter, 0);
        EXPECT_EQ(counter2.counter, 0);
        EXPECT_EQ(object->counter, 0);
        EXPECT_EQ((*object).counter, 0);
    }

    EXPECT_EQ(counter1.counter, -1);
    EXPECT_EQ(counter2.counter, 0);
}

TEST(AngelScriptOwnedObject, MoveAssignment) {
    as::ScriptTestObject counter1, counter2;
    EXPECT_EQ(counter1.counter, 1);
    EXPECT_EQ(counter2.counter, 1);

    {
        as::OwnedObject one(&counter1);
        ASSERT_TRUE(one);
        ASSERT_EQ(one.operator->(), &counter1);
        EXPECT_EQ(counter1.counter, 1);
        EXPECT_EQ(counter2.counter, 1);
        EXPECT_EQ(one->counter, 1);
        EXPECT_EQ((*one).counter, 1);

        as::OwnedObject two(&counter2);
        ASSERT_TRUE(two);
        ASSERT_EQ(two.operator->(), &counter2);
        EXPECT_EQ(counter1.counter, 1);
        EXPECT_EQ(counter2.counter, 1);
        EXPECT_EQ(two->counter, 1);
        EXPECT_EQ((*two).counter, 1);

        ASSERT_TRUE((one = std::move(two)).operator bool());
        ASSERT_EQ(one.operator->(), &counter2);
        ASSERT_EQ(two.operator->(), &counter2);
        EXPECT_EQ(counter1.counter, 0);
        EXPECT_EQ(counter2.counter, 2);
        EXPECT_EQ(one->counter, 2);
        EXPECT_EQ((*one).counter, 2);
        EXPECT_EQ(two->counter, 2);
        EXPECT_EQ((*two).counter, 2);

        ASSERT_TRUE((two = std::move(one)).operator bool());
        ASSERT_EQ(one.operator->(), &counter2);
        ASSERT_EQ(two.operator->(), &counter2);
        EXPECT_EQ(counter1.counter, 0);
        EXPECT_EQ(counter2.counter, 2);
        EXPECT_EQ(one->counter, 2);
        EXPECT_EQ((*one).counter, 2);
        EXPECT_EQ(two->counter, 2);
        EXPECT_EQ((*two).counter, 2);

        two = std::move(as::OwnedObject<as::ScriptTestObject>());
        ASSERT_TRUE(one);
        EXPECT_FALSE(two);
        EXPECT_EQ(counter1.counter, 0);
        EXPECT_EQ(counter2.counter, 1);
        EXPECT_EQ(one->counter, 1);
        EXPECT_EQ((*one).counter, 1);

        ASSERT_TRUE((two = std::move(one)).operator bool());
        ASSERT_EQ(one.operator->(), &counter2);
        ASSERT_EQ(two.operator->(), &counter2);
        EXPECT_EQ(counter1.counter, 0);
        EXPECT_EQ(counter2.counter, 2);
        EXPECT_EQ(one->counter, 2);
        EXPECT_EQ((*one).counter, 2);
        EXPECT_EQ(two->counter, 2);
        EXPECT_EQ((*two).counter, 2);
    }

    EXPECT_EQ(counter1.counter, 0);
    EXPECT_EQ(counter2.counter, 0);
}
