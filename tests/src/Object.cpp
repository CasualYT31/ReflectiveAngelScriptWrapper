#include <AngelScriptWrapper/Object.hpp>
#include <AngelScriptWrapperTests/ScriptTestObject.hpp>
#include <gtest/gtest.h>

TEST(AngelScriptObject, DefaultConstructor) {
    as::Object<as::ScriptTestObject> object;
    EXPECT_FALSE(object);
    EXPECT_FALSE(object.operator->());
}

TEST(AngelScriptObject, PointerConstructor) {
    as::ScriptTestObject counter;
    EXPECT_EQ(counter.counter, 1);

    {
        as::Object object(&counter);
        ASSERT_TRUE(object);
        ASSERT_EQ(object.operator->(), &counter);
        EXPECT_EQ(counter.counter, 2);
        EXPECT_EQ(object->counter, 2);
        EXPECT_EQ((*object).counter, 2);
    }

    EXPECT_EQ(counter.counter, 1);
}

TEST(AngelScriptObject, CopyConstructor) {
    as::ScriptTestObject counter;
    EXPECT_EQ(counter.counter, 1);

    {
        as::Object original(&counter);
        ASSERT_TRUE(original);
        ASSERT_EQ(original.operator->(), &counter);
        EXPECT_EQ(counter.counter, 2);
        EXPECT_EQ(original->counter, 2);
        EXPECT_EQ((*original).counter, 2);

        {
            as::Object copy(original);
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

TEST(AngelScriptObject, CopyOwnerConstructor) {
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
            as::Object copy(original);
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

TEST(AngelScriptObject, MoveConstructor) {
    as::ScriptTestObject counter;
    EXPECT_EQ(counter.counter, 1);

    {
        as::Object original(&counter);
        ASSERT_TRUE(original);
        ASSERT_EQ(original.operator->(), &counter);
        EXPECT_EQ(counter.counter, 2);
        EXPECT_EQ(original->counter, 2);
        EXPECT_EQ((*original).counter, 2);

        {
            as::Object moved(std::move(original));
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

TEST(AngelScriptObject, PointerAssignment) {
    as::ScriptTestObject counter1, counter2;
    EXPECT_EQ(counter1.counter, 1);
    EXPECT_EQ(counter2.counter, 1);

    {
        as::Object object(&counter1);
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

TEST(AngelScriptObject, CopyAssignment) {
    as::ScriptTestObject counter1, counter2;
    EXPECT_EQ(counter1.counter, 1);
    EXPECT_EQ(counter2.counter, 1);

    {
        as::Object one(&counter1);
        ASSERT_TRUE(one);
        ASSERT_EQ(one.operator->(), &counter1);
        EXPECT_EQ(counter1.counter, 2);
        EXPECT_EQ(counter2.counter, 1);
        EXPECT_EQ(one->counter, 2);
        EXPECT_EQ((*one).counter, 2);

        as::Object two(&counter2);
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

        two = as::Object<as::ScriptTestObject>();
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

TEST(AngelScriptObject, CopyOwnerAssignment) {
    as::ScriptTestObject counter1, counter2;
    EXPECT_EQ(counter1.counter, 1);
    EXPECT_EQ(counter2.counter, 1);

    {
        as::Object one(&counter1);
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

TEST(AngelScriptObject, MoveAssignment) {
    as::ScriptTestObject counter1, counter2;
    EXPECT_EQ(counter1.counter, 1);
    EXPECT_EQ(counter2.counter, 1);

    {
        as::Object one(&counter1);
        ASSERT_TRUE(one);
        ASSERT_EQ(one.operator->(), &counter1);
        EXPECT_EQ(counter1.counter, 2);
        EXPECT_EQ(counter2.counter, 1);
        EXPECT_EQ(one->counter, 2);
        EXPECT_EQ((*one).counter, 2);

        as::Object two(&counter2);
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

        two = std::move(as::Object<as::ScriptTestObject>());
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
