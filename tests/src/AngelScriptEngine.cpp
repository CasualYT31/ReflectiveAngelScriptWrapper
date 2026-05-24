#include <AngelScriptWrapper/AngelScriptEngine.hpp>
#include <gtest/gtest.h>

TEST(AngelScriptEngine, PrintHelloWorld) {
    as::test("Hello, world!");
}
