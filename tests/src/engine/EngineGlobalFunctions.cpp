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

    AS_NAMESPACE_QUALIFIER asDWORD callConv = -1;
    ASSERT_GE(engine.RegisterGlobalFunction<^^add>(callConv), 0);
    ASSERT_EQ(callConv, AS_NAMESPACE_QUALIFIER asCALL_CDECL);

    int result = 0;
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return add(5, 6);", &result, 4), 0);

    EXPECT_EQ(result, 11);
}

TEST(AngelScriptEngineGlobalFunctions, SimpleMethod) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());
    as::SetMessageCallback(engine);
    Global obj;

    AS_NAMESPACE_QUALIFIER asDWORD callConv = -1;
    ASSERT_GE(engine.RegisterGlobalFunction<^^Global::add>(callConv, &obj), 0);
    ASSERT_EQ(callConv, AS_NAMESPACE_QUALIFIER asCALL_THISCALL_ASGLOBAL);

    int result = 0;
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return add(5, 6);", &result, 4), 0);

    EXPECT_EQ(result, 11);
}

TEST(AngelScriptEngineGlobalFunctions, SimpleFunctionOverload) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());

    constexpr auto intendedAdd = as::FindOverload<^^::, double, false, const double, const double>("add2");
    AS_NAMESPACE_QUALIFIER asDWORD callConv = -1;
    ASSERT_GE(engine.RegisterGlobalFunction<intendedAdd>(callConv), 0);
    ASSERT_EQ(callConv, AS_NAMESPACE_QUALIFIER asCALL_CDECL);

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
    AS_NAMESPACE_QUALIFIER asDWORD callConv = -1;
    ASSERT_GE(engine.RegisterGlobalFunction<intendedAdd>(callConv, &obj), 0);
    ASSERT_EQ(callConv, AS_NAMESPACE_QUALIFIER asCALL_THISCALL_ASGLOBAL);

    double result = 0;
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return add2(5.5, 2.5);", &result, 11), 0);

    EXPECT_DOUBLE_EQ(result, 3.0);
}

// TODO: working on Ubuntu WSL with 64-bit compiler, so can't test StdCall for the time being.

// __attribute__((stdcall)) double square[[ = as::Name("s"), = as::StdCall ]](double a) {
//     return a * a;
// }

// TEST(AngelScriptEngineGlobalFunctions, RenameStdCallFunction) {
//     as::Engine engine;
//     ASSERT_TRUE(engine.HasEngine());

// AS_NAMESPACE_QUALIFIER asDWORD callConv = -1;
// ASSERT_GE(engine.RegisterGlobalFunction<^^square>(callConv), 0);
// ASSERT_EQ(callConv, AS_NAMESPACE_QUALIFIER asCALL_STDCALL);

// double result = 0;
// ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return s(7.8);", &result, 11), 0);

// EXPECT_DOUBLE_EQ(result, 60.84);
// }

double cSquare[[ = as::Name("s"), = as::CDecl ]](double a) {
    return a * a;
}

TEST(AngelScriptEngineGlobalFunctions, RenameCDeclFunction) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());
    engine.SetDefaultCallConvention(AS_NAMESPACE_QUALIFIER asCALL_STDCALL);

    AS_NAMESPACE_QUALIFIER asDWORD callConv = -1;
    ASSERT_GE(engine.RegisterGlobalFunction<^^cSquare>(callConv), 0);
    ASSERT_EQ(callConv, AS_NAMESPACE_QUALIFIER asCALL_CDECL);

    double result = 0;
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return s(7.8);", &result, 11), 0);

    EXPECT_DOUBLE_EQ(result, 60.84);
}

void bitwiseOr[[= as::ObjFirst]](std::uint8_t* obj, const std::uint8_t a, const std::uint8_t b, std::uint8_t& c) {
    *obj = c = a | b;
}

/* TODO: I don't think OBJFRIST or OBJLAST work with global functions. Output of the following test:

No matching signatures to 'bitwiseOr(const int, const int, uint8)'
Candidates are:
void bitwiseOr(uint8&out, uint8, uint8, uint8&out)

Make sure of this first, then repurpose the test to try out &out.
Not convinced the above is the right signature for the C++ function.
*/
TEST(AngelScriptEngineGlobalFunctions, SimpleFunction2) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());
    as::SetMessageCallback(engine);

    std::uint8_t r = 0;
    AS_NAMESPACE_QUALIFIER asDWORD callConv = -1;
    ASSERT_GE(engine.RegisterGlobalFunction<^^bitwiseOr>(callConv, &r), 0);
    ASSERT_EQ(callConv, AS_NAMESPACE_QUALIFIER asCALL_CDECL_OBJFIRST);

    std::uint8_t result = 0;
    ASSERT_GE(
        AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "uint8 r = 0; bitwiseOr(4, 8, r); return r;", &result, 6), 0
    );

    EXPECT_EQ(result, 12);
    EXPECT_EQ(r, 12);
}
