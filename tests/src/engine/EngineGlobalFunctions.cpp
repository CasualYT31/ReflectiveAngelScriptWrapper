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

void bitwiseOrRef(const std::uint8_t a, const std::uint8_t b, std::uint8_t& c) {
    c = a | b;
}

TEST(AngelScriptEngineGlobalFunctions, OutRefUsingRef) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());
    as::SetMessageCallback(engine);

    AS_NAMESPACE_QUALIFIER asDWORD callConv = -1;
    ASSERT_GE(engine.RegisterGlobalFunction<^^bitwiseOrRef>(callConv), 0);
    ASSERT_EQ(callConv, AS_NAMESPACE_QUALIFIER asCALL_CDECL);

    std::uint8_t result = 0;
    ASSERT_GE(
        AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "uint8 r = 0; bitwiseOrRef(4, 8, r); return r;", &result, 6), 0
    );

    EXPECT_EQ(result, 12);
}

void bitwiseOrPtr(const std::uint8_t a, const std::uint8_t b, std::uint8_t* c) {
    *c = a | b;
}

TEST(AngelScriptEngineGlobalFunctions, OutRefUsingPtr) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());
    as::SetMessageCallback(engine);

    AS_NAMESPACE_QUALIFIER asDWORD callConv = -1;
    ASSERT_GE(engine.RegisterGlobalFunction<^^bitwiseOrPtr>(callConv), 0);
    ASSERT_EQ(callConv, AS_NAMESPACE_QUALIFIER asCALL_CDECL);

    std::uint8_t result = 0;
    ASSERT_GE(
        AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "uint8 r = 0; bitwiseOrPtr(4, 8, r); return r;", &result, 6), 0
    );

    EXPECT_EQ(result, 12);
}

std::uint8_t bitwiseOrIn(std::uint8_t const& a, std::uint8_t const* b, std::uint8_t const* const c) {
    return a | *b | *c;
}

TEST(AngelScriptEngineGlobalFunctions, InRefAndPtrs) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());
    as::SetMessageCallback(engine);

    AS_NAMESPACE_QUALIFIER asDWORD callConv = -1;
    ASSERT_GE(engine.RegisterGlobalFunction<^^bitwiseOrIn>(callConv), 0);
    ASSERT_EQ(callConv, AS_NAMESPACE_QUALIFIER asCALL_CDECL);

    std::uint8_t result = 0;
    ASSERT_GE(
        AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "uint8 r = 16; return bitwiseOrIn(4, 8, r);", &result, 6), 0
    );

    EXPECT_EQ(result, 28);
}

// TODO: we need to test how ref counting works, let's implement a proper ref-counted type.
//       I will copy my superclass for this from my CW project.
//       What happens if a function accepts a ref type by value? Is this even supported?
//       Same for if it accepts it by reference? Do we need the AsHandle annotation?
//       Play around with the "+" modifier.

TEST(AngelScriptEngineGlobalFunctions, RefTypeParams) {}
