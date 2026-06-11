#include <AngelScriptWrapper/Engine.hpp>
#include <AngelScriptWrapperTests/ScriptDebugging.hpp>
#include <gtest/gtest.h>
#include <scripthelper.h>

using MyTypedef = std::uint64_t;

typedef bool MyFlag;

TEST(AngelScriptEngineTypedefs, Registration) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());

    ASSERT_LT(
        AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "MyTypedef number = 1000000000; MyFlag flag = true;"), 0
    );

    ASSERT_GE(engine.RegisterTypedef<^^MyTypedef>(), 0);
    ASSERT_GE(engine.RegisterTypedef<^^MyFlag>(), 0);

    ASSERT_GE(
        AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "MyTypedef number = 1000000000; MyFlag flag = true;"), 0
    );
}

using One[[= as::Name("MyOne")]] = float;

typedef double Two[[= as::Name("MyTwo")]];

TEST(AngelScriptEngineTypedefs, RegistrationWithRenameAnnotations) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());

    ASSERT_LT(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "MyOne number = 0.23; MyTwo fp = 123.213123123;"), 0);

    ASSERT_GE(engine.RegisterTypedef<^^One>(), 0);
    ASSERT_GE(engine.RegisterTypedef<^^Two>(), 0);

    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "MyOne number = 0.23; MyTwo fp = 123.213123123;"), 0);
}
