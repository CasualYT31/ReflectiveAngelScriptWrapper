#include <AngelScriptWrapper/SharedObject.hpp>
#include <AngelScriptWrapperTests/ScriptTestObject.hpp>
#include <gtest/gtest.h>

TEST(AngelScriptSharedObject, DefaultConstructor) {
    as::SharedObject<as::ScriptTestObject> object;
    EXPECT_FALSE(object);
    EXPECT_FALSE(object.operator->());
}

TEST(AngelScriptSharedObject, PointerConstructor) {
    as::ScriptTestObject counter;
    EXPECT_EQ(counter.counter, 1);

    {
        as::SharedObject object(&counter);
        ASSERT_TRUE(object);
        ASSERT_EQ(object.operator->(), &counter);
        EXPECT_EQ(counter.counter, 2);
        EXPECT_EQ(object->counter, 2);
        EXPECT_EQ((*object).counter, 2);
    }

    EXPECT_EQ(counter.counter, 1);
}

TEST(AngelScriptSharedObject, CopyConstructor) {
    as::ScriptTestObject counter;
    EXPECT_EQ(counter.counter, 1);

    {
        as::SharedObject original(&counter);
        ASSERT_TRUE(original);
        ASSERT_EQ(original.operator->(), &counter);
        EXPECT_EQ(counter.counter, 2);
        EXPECT_EQ(original->counter, 2);
        EXPECT_EQ((*original).counter, 2);

        {
            as::SharedObject copy(original);
            ASSERT_TRUE(copy);
            ASSERT_EQ(copy.operator->(), &counter);
            ASSERT_EQ(copy.operator->(), original.operator->());
            EXPECT_EQ(counter.counter, 3);
            EXPECT_EQ(copy->counter, 3);
            EXPECT_EQ((*copy).counter, 3);
            EXPECT_EQ(original->counter, 3);
            EXPECT_EQ((*original).counter, 3);
        }

        ASSERT_TRUE(original);
        ASSERT_EQ(original.operator->(), &counter);
        EXPECT_EQ(counter.counter, 2);
        EXPECT_EQ(original->counter, 2);
        EXPECT_EQ((*original).counter, 2);
    }

    EXPECT_EQ(counter.counter, 1);
}

TEST(AngelScriptSharedObject, CopyOwnerConstructor) {
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
            as::SharedObject copy(original);
            ASSERT_TRUE(copy);
            ASSERT_EQ(copy.operator->(), &counter);
            ASSERT_EQ(copy.operator->(), original.operator->());
            EXPECT_EQ(counter.counter, 2);
            EXPECT_EQ(copy->counter, 2);
            EXPECT_EQ((*copy).counter, 2);
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

TEST(AngelScriptSharedObject, MoveConstructor) {
    as::ScriptTestObject counter;
    EXPECT_EQ(counter.counter, 1);

    {
        as::SharedObject original(&counter);
        ASSERT_TRUE(original);
        ASSERT_EQ(original.operator->(), &counter);
        EXPECT_EQ(counter.counter, 2);
        EXPECT_EQ(original->counter, 2);
        EXPECT_EQ((*original).counter, 2);

        {
            as::SharedObject moved(std::move(original));
            ASSERT_TRUE(moved);
            ASSERT_EQ(moved.operator->(), &counter);
            ASSERT_EQ(moved.operator->(), original.operator->());
            EXPECT_EQ(counter.counter, 3);
            EXPECT_EQ(moved->counter, 3);
            EXPECT_EQ((*moved).counter, 3);
            EXPECT_EQ(original->counter, 3);
            EXPECT_EQ((*original).counter, 3);
        }

        ASSERT_TRUE(original);
        ASSERT_EQ(original.operator->(), &counter);
        EXPECT_EQ(counter.counter, 2);
        EXPECT_EQ(original->counter, 2);
        EXPECT_EQ((*original).counter, 2);
    }

    EXPECT_EQ(counter.counter, 1);
}

TEST(AngelScriptSharedObject, PointerAssignment) {
    as::ScriptTestObject counter1, counter2;
    EXPECT_EQ(counter1.counter, 1);
    EXPECT_EQ(counter2.counter, 1);

    {
        as::SharedObject object(&counter1);
        ASSERT_TRUE(object);
        ASSERT_EQ(object.operator->(), &counter1);
        EXPECT_EQ(counter1.counter, 2);
        EXPECT_EQ(counter2.counter, 1);
        EXPECT_EQ(object->counter, 2);
        EXPECT_EQ((*object).counter, 2);

        ASSERT_TRUE((object = &counter2).operator bool());
        ASSERT_EQ(object.operator->(), &counter2);
        EXPECT_EQ(counter1.counter, 1);
        EXPECT_EQ(counter2.counter, 2);
        EXPECT_EQ(object->counter, 2);
        EXPECT_EQ((*object).counter, 2);

        EXPECT_FALSE((object = nullptr).operator bool());
        EXPECT_EQ(object.operator->(), nullptr);
        EXPECT_EQ(counter1.counter, 1);
        EXPECT_EQ(counter2.counter, 1);

        ASSERT_TRUE((object = &counter1).operator bool());
        ASSERT_EQ(object.operator->(), &counter1);
        EXPECT_EQ(counter1.counter, 2);
        EXPECT_EQ(counter2.counter, 1);
        EXPECT_EQ(object->counter, 2);
        EXPECT_EQ((*object).counter, 2);
    }

    EXPECT_EQ(counter1.counter, 1);
    EXPECT_EQ(counter2.counter, 1);
}

TEST(AngelScriptSharedObject, CopyAssignment) {
    as::ScriptTestObject counter1, counter2;
    EXPECT_EQ(counter1.counter, 1);
    EXPECT_EQ(counter2.counter, 1);

    {
        as::SharedObject one(&counter1);
        ASSERT_TRUE(one);
        ASSERT_EQ(one.operator->(), &counter1);
        EXPECT_EQ(counter1.counter, 2);
        EXPECT_EQ(counter2.counter, 1);
        EXPECT_EQ(one->counter, 2);
        EXPECT_EQ((*one).counter, 2);

        as::SharedObject two(&counter2);
        ASSERT_TRUE(two);
        ASSERT_EQ(two.operator->(), &counter2);
        EXPECT_EQ(counter1.counter, 2);
        EXPECT_EQ(counter2.counter, 2);
        EXPECT_EQ(two->counter, 2);
        EXPECT_EQ((*two).counter, 2);

        ASSERT_TRUE((one = two).operator bool());
        ASSERT_EQ(one.operator->(), &counter2);
        ASSERT_EQ(two.operator->(), &counter2);
        EXPECT_EQ(counter1.counter, 1);
        EXPECT_EQ(counter2.counter, 3);
        EXPECT_EQ(one->counter, 3);
        EXPECT_EQ((*one).counter, 3);
        EXPECT_EQ(two->counter, 3);
        EXPECT_EQ((*two).counter, 3);

        ASSERT_TRUE((two = one).operator bool());
        ASSERT_EQ(one.operator->(), &counter2);
        ASSERT_EQ(two.operator->(), &counter2);
        EXPECT_EQ(counter1.counter, 1);
        EXPECT_EQ(counter2.counter, 3);
        EXPECT_EQ(one->counter, 3);
        EXPECT_EQ((*one).counter, 3);
        EXPECT_EQ(two->counter, 3);
        EXPECT_EQ((*two).counter, 3);

        two = as::SharedObject<as::ScriptTestObject>();
        ASSERT_TRUE(one);
        EXPECT_FALSE(two);
        EXPECT_EQ(counter1.counter, 1);
        EXPECT_EQ(counter2.counter, 2);
        EXPECT_EQ(one->counter, 2);
        EXPECT_EQ((*one).counter, 2);

        ASSERT_TRUE((two = one).operator bool());
        ASSERT_EQ(one.operator->(), &counter2);
        ASSERT_EQ(two.operator->(), &counter2);
        EXPECT_EQ(counter1.counter, 1);
        EXPECT_EQ(counter2.counter, 3);
        EXPECT_EQ(one->counter, 3);
        EXPECT_EQ((*one).counter, 3);
        EXPECT_EQ(two->counter, 3);
        EXPECT_EQ((*two).counter, 3);
    }

    EXPECT_EQ(counter1.counter, 1);
    EXPECT_EQ(counter2.counter, 1);
}

TEST(AngelScriptSharedObject, CopyOwnerAssignment) {
    as::ScriptTestObject counter1, counter2;
    EXPECT_EQ(counter1.counter, 1);
    EXPECT_EQ(counter2.counter, 1);

    {
        as::SharedObject one(&counter1);
        ASSERT_TRUE(one);
        ASSERT_EQ(one.operator->(), &counter1);
        EXPECT_EQ(counter1.counter, 2);
        EXPECT_EQ(counter2.counter, 1);
        EXPECT_EQ(one->counter, 2);
        EXPECT_EQ((*one).counter, 2);

        as::OwnedObject two(&counter2);
        ASSERT_TRUE(two);
        ASSERT_EQ(two.operator->(), &counter2);
        EXPECT_EQ(counter1.counter, 2);
        EXPECT_EQ(counter2.counter, 1);
        EXPECT_EQ(two->counter, 1);
        EXPECT_EQ((*two).counter, 1);

        ASSERT_TRUE((one = two).operator bool());
        ASSERT_EQ(one.operator->(), &counter2);
        ASSERT_EQ(two.operator->(), &counter2);
        EXPECT_EQ(counter1.counter, 1);
        EXPECT_EQ(counter2.counter, 2);
        EXPECT_EQ(one->counter, 2);
        EXPECT_EQ((*one).counter, 2);
        EXPECT_EQ(two->counter, 2);
        EXPECT_EQ((*two).counter, 2);

        two = std::move(as::OwnedObject<as::ScriptTestObject>());
        ASSERT_TRUE(one);
        EXPECT_FALSE(two);
        EXPECT_EQ(counter1.counter, 1);
        EXPECT_EQ(counter2.counter, 1);
        EXPECT_EQ(one->counter, 1);
        EXPECT_EQ((*one).counter, 1);

        EXPECT_FALSE((one = two).operator bool());
        EXPECT_EQ(counter1.counter, 1);
        EXPECT_EQ(counter2.counter, 0);
    }

    EXPECT_EQ(counter1.counter, 1);
    EXPECT_EQ(counter2.counter, 0);
}

TEST(AngelScriptSharedObject, MoveAssignment) {
    as::ScriptTestObject counter1, counter2;
    EXPECT_EQ(counter1.counter, 1);
    EXPECT_EQ(counter2.counter, 1);

    {
        as::SharedObject one(&counter1);
        ASSERT_TRUE(one);
        ASSERT_EQ(one.operator->(), &counter1);
        EXPECT_EQ(counter1.counter, 2);
        EXPECT_EQ(counter2.counter, 1);
        EXPECT_EQ(one->counter, 2);
        EXPECT_EQ((*one).counter, 2);

        as::SharedObject two(&counter2);
        ASSERT_TRUE(two);
        ASSERT_EQ(two.operator->(), &counter2);
        EXPECT_EQ(counter1.counter, 2);
        EXPECT_EQ(counter2.counter, 2);
        EXPECT_EQ(two->counter, 2);
        EXPECT_EQ((*two).counter, 2);

        ASSERT_TRUE((one = std::move(two)).operator bool());
        ASSERT_EQ(one.operator->(), &counter2);
        ASSERT_EQ(two.operator->(), &counter2);
        EXPECT_EQ(counter1.counter, 1);
        EXPECT_EQ(counter2.counter, 3);
        EXPECT_EQ(one->counter, 3);
        EXPECT_EQ((*one).counter, 3);
        EXPECT_EQ(two->counter, 3);
        EXPECT_EQ((*two).counter, 3);

        ASSERT_TRUE((two = std::move(one)).operator bool());
        ASSERT_EQ(one.operator->(), &counter2);
        ASSERT_EQ(two.operator->(), &counter2);
        EXPECT_EQ(counter1.counter, 1);
        EXPECT_EQ(counter2.counter, 3);
        EXPECT_EQ(one->counter, 3);
        EXPECT_EQ((*one).counter, 3);
        EXPECT_EQ(two->counter, 3);
        EXPECT_EQ((*two).counter, 3);

        two = std::move(as::SharedObject<as::ScriptTestObject>());
        ASSERT_TRUE(one);
        EXPECT_FALSE(two);
        EXPECT_EQ(counter1.counter, 1);
        EXPECT_EQ(counter2.counter, 2);
        EXPECT_EQ(one->counter, 2);
        EXPECT_EQ((*one).counter, 2);

        ASSERT_TRUE((two = std::move(one)).operator bool());
        ASSERT_EQ(one.operator->(), &counter2);
        ASSERT_EQ(two.operator->(), &counter2);
        EXPECT_EQ(counter1.counter, 1);
        EXPECT_EQ(counter2.counter, 3);
        EXPECT_EQ(one->counter, 3);
        EXPECT_EQ((*one).counter, 3);
        EXPECT_EQ(two->counter, 3);
        EXPECT_EQ((*two).counter, 3);
    }

    EXPECT_EQ(counter1.counter, 1);
    EXPECT_EQ(counter2.counter, 1);
}
