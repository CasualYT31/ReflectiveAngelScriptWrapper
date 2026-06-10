#include <AngelScriptWrapper/Engine.hpp>
#include <AngelScriptWrapperTests/ScriptDebugging.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <scripthelper.h>

using namespace testing;

struct Empty {};

TEST(AngelScriptEngineInterfaces, RegisterEmptyInterface) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());

    ASSERT_GE(engine.RegisterInterface<^^Empty>(), 0);

    EXPECT_EQ(engine.Ptr()->RegisterInterface("Empty"), AS_NAMESPACE_QUALIFIER asALREADY_REGISTERED);
}

struct A {};

struct B : A {};

struct C : B {};

TEST(AngelScriptEngineInterfaces, RegisterRecursivelyByDefault) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());

    ASSERT_GE((engine.RegisterInterface<^^C>()), 0);

    EXPECT_EQ(engine.Ptr()->RegisterInterface("C"), AS_NAMESPACE_QUALIFIER asALREADY_REGISTERED);
    EXPECT_EQ(engine.Ptr()->RegisterInterface("A"), AS_NAMESPACE_QUALIFIER asALREADY_REGISTERED);
    EXPECT_EQ(engine.Ptr()->RegisterInterface("B"), AS_NAMESPACE_QUALIFIER asALREADY_REGISTERED);
}

TEST(AngelScriptEngineInterfaces, RegisterNonRecursively) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());

    ASSERT_GE((engine.RegisterInterface<^^C, false>()), 0);

    EXPECT_EQ(engine.Ptr()->RegisterInterface("C"), AS_NAMESPACE_QUALIFIER asALREADY_REGISTERED);
    EXPECT_GE(engine.Ptr()->RegisterInterface("A"), 0);
    EXPECT_GE(engine.Ptr()->RegisterInterface("B"), 0);
}

namespace original {
struct Basic {
    virtual int getNumber() = 0;
};
} // namespace original

int interfaceTest(AS_NAMESPACE_QUALIFIER asIScriptObject* const scriptObject[[= as::Interface(^^original::Basic)]]) {
    if (const auto asType = scriptObject->GetObjectType()) {
        if (const auto func = asType->GetMethodByDecl(as::GetFuncDecl<^^original::Basic::getNumber>().data())) {
            if (const auto engine = func->GetEngine()) {
                if (const auto ctx = engine->CreateContext()) {
                    if (ctx->Prepare(func) >= 0) {
                        if (ctx->SetObject(scriptObject) >= 0) {
                            if (ctx->Execute() == AS_NAMESPACE_QUALIFIER asEXECUTION_FINISHED) {
                                return static_cast<int>(ctx->GetReturnDWord());
                            } else {
                                return -7;
                            }
                        } else {
                            return -6;
                        }
                    } else {
                        return -5;
                    }
                } else {
                    return -4;
                }
            } else {
                return -3;
            }
        } else {
            return -2;
        }
    } else {
        return -1;
    }
}

TEST(AngelScriptEngineInterfaces, RegisterBasicInterface) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());

    std::vector<std::string> messages;
    ASSERT_GE((as::SetMessageCallback(engine, &messages)), 0);

    ASSERT_GE((engine.RegisterInterface<^^original::Basic>()), 0);

    EXPECT_EQ(engine.Ptr()->RegisterInterface("Basic"), AS_NAMESPACE_QUALIFIER asALREADY_REGISTERED);

    ASSERT_GE(engine.RegisterGlobalFunction<^^interfaceTest>(), 0);

    AS_NAMESPACE_QUALIFIER asIScriptModule* mod = engine.Ptr()->GetModule("test", AS_NAMESPACE_QUALIFIER asGM_ALWAYS_CREATE);
    ASSERT_TRUE(mod);

    ASSERT_GE(mod->AddScriptSection("test.as", "class MyImpl : Basic {}"), 0);

    ASSERT_LT(mod->Build(), 0);

    EXPECT_THAT(messages, ElementsAre("Missing implementation of 'int Basic::getNumber()'"));

    ASSERT_GE(engine.Ptr()->DiscardModule("test"), 0);

    mod = engine.Ptr()->GetModule("test", AS_NAMESPACE_QUALIFIER asGM_ALWAYS_CREATE);
    ASSERT_TRUE(mod);

    ASSERT_GE(
        mod->AddScriptSection(
            "test.as",
            "class MyImpl : Basic { int getNumber() { return 6; } }"
            "int main() { MyImpl test; return test.getNumber(); }"
            "int scriptInterfaceTest() { MyImpl test; return interfaceTest(test); }"
        ),
        0
    );

    ASSERT_GE(mod->Build(), 0);

    AS_NAMESPACE_QUALIFIER asIScriptContext* ctx = engine.Ptr()->CreateContext();
    ASSERT_TRUE(ctx);

    AS_NAMESPACE_QUALIFIER asIScriptFunction* func = mod->GetFunctionByDecl("int main()");
    ASSERT_TRUE(func);

    ASSERT_GE(ctx->Prepare(func), 0);

    ASSERT_EQ(ctx->Execute(), AS_NAMESPACE_QUALIFIER asEXECUTION_FINISHED);

    const auto res1 = static_cast<int>(ctx->GetReturnDWord());
    EXPECT_EQ(res1, 6);

    func = mod->GetFunctionByDecl("int scriptInterfaceTest()");
    ASSERT_TRUE(func);

    ASSERT_GE(ctx->Prepare(func), 0);

    ASSERT_EQ(ctx->Execute(), AS_NAMESPACE_QUALIFIER asEXECUTION_FINISHED);

    const auto res2 = static_cast<int>(ctx->GetReturnDWord());
    EXPECT_EQ(res2, 6);
}

struct Extended : original::Basic {
    virtual float getFloat() const = 0;
};

TEST(AngelScriptEngineInterfaces, RegisterExtendedInterface) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());

    std::vector<std::string> messages;
    ASSERT_GE((as::SetMessageCallback(engine, &messages)), 0);

    ASSERT_GE((engine.RegisterInterface<^^Extended>()), 0);

    EXPECT_EQ(engine.Ptr()->RegisterInterface("Basic"), AS_NAMESPACE_QUALIFIER asALREADY_REGISTERED);
    EXPECT_EQ(engine.Ptr()->RegisterInterface("Extended"), AS_NAMESPACE_QUALIFIER asALREADY_REGISTERED);

    AS_NAMESPACE_QUALIFIER asIScriptModule* mod = engine.Ptr()->GetModule("test", AS_NAMESPACE_QUALIFIER asGM_ALWAYS_CREATE);
    ASSERT_TRUE(mod);

    ASSERT_GE(mod->AddScriptSection("test.as", "class MyImpl : Basic {} class MyExtImpl : Extended {}"), 0);

    ASSERT_LT(mod->Build(), 0);

    EXPECT_THAT(
        messages,
        ElementsAre(
            "Missing implementation of 'int Basic::getNumber()'",
            "Missing implementation of 'float Extended::getFloat() const'",
            "Missing implementation of 'int Extended::getNumber()'"
        )
    );
}

namespace other {
struct[[= as::Name("RenamedBasic")]] Basic : Extended {
    Basic(int ignored) {};

    virtual ~Basic() = 0;

    int ignoredInt = 0;

    void ignoredMethod() {};

    virtual void ignoredVirtualMethod() {};

    // Also ignored:
    virtual other::Basic& operator+(other::Basic const&) = 0;

protected:
    virtual void ignored() = 0;
};
} // namespace other

TEST(AngelScriptEngineInterfaces, RegisterRenamedInterface) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());

    std::vector<std::string> messages;
    ASSERT_GE((as::SetMessageCallback(engine, &messages)), 0);

    ASSERT_GE((engine.RegisterInterface<^^other::Basic>()), 0);

    EXPECT_EQ(engine.Ptr()->RegisterInterface("Basic"), AS_NAMESPACE_QUALIFIER asALREADY_REGISTERED);
    EXPECT_EQ(engine.Ptr()->RegisterInterface("Extended"), AS_NAMESPACE_QUALIFIER asALREADY_REGISTERED);
    EXPECT_EQ(engine.Ptr()->RegisterInterface("RenamedBasic"), AS_NAMESPACE_QUALIFIER asALREADY_REGISTERED);

    AS_NAMESPACE_QUALIFIER asIScriptModule* mod = engine.Ptr()->GetModule("test", AS_NAMESPACE_QUALIFIER asGM_ALWAYS_CREATE);
    ASSERT_TRUE(mod);

    ASSERT_GE(
        mod->AddScriptSection(
            "test.as", "class MyImpl : Basic {} class MyExtImpl : Extended {} class MyRenamedImpl : RenamedBasic {}"
        ),
        0
    );

    ASSERT_LT(mod->Build(), 0);

    EXPECT_THAT(
        messages,
        ElementsAre(
            "Missing implementation of 'int Basic::getNumber()'",
            "Missing implementation of 'float Extended::getFloat() const'",
            "Missing implementation of 'int Extended::getNumber()'",
            "Missing implementation of 'float RenamedBasic::getFloat() const'",
            "Missing implementation of 'int RenamedBasic::getNumber()'"
        )
    );
}

namespace incorrect {
struct Basic : Extended {};
} // namespace incorrect

TEST(AngelScriptEngineInterfaces, RegisterInterfaceWithTakenName) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());

    ASSERT_EQ((engine.RegisterInterface<^^incorrect::Basic>()), AS_NAMESPACE_QUALIFIER asALREADY_REGISTERED);
}

struct DiamondTip {
    virtual double getDouble() = 0;
};

struct DiamondLeft : DiamondTip {
    virtual std::int8_t getChar() = 0;
};

struct DiamondRight : DiamondTip {
    virtual std::int8_t getChar() = 0;
};

struct Diamond : DiamondLeft, DiamondRight {
    virtual double getDouble(double overloaded) = 0;
};

TEST(AngelScriptEngineInterfaces, RegisterDiamondInterface) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());

    std::vector<std::string> messages;
    ASSERT_GE((as::SetMessageCallback(engine, &messages)), 0);

    ASSERT_GE((engine.RegisterInterface<^^Diamond>()), 0);

    EXPECT_EQ(engine.Ptr()->RegisterInterface("Diamond"), AS_NAMESPACE_QUALIFIER asALREADY_REGISTERED);
    EXPECT_EQ(engine.Ptr()->RegisterInterface("DiamondRight"), AS_NAMESPACE_QUALIFIER asALREADY_REGISTERED);
    EXPECT_EQ(engine.Ptr()->RegisterInterface("DiamondLeft"), AS_NAMESPACE_QUALIFIER asALREADY_REGISTERED);
    EXPECT_EQ(engine.Ptr()->RegisterInterface("DiamondTip"), AS_NAMESPACE_QUALIFIER asALREADY_REGISTERED);

    AS_NAMESPACE_QUALIFIER asIScriptModule* mod = engine.Ptr()->GetModule("test", AS_NAMESPACE_QUALIFIER asGM_ALWAYS_CREATE);
    ASSERT_TRUE(mod);

    ASSERT_GE(mod->AddScriptSection("test.as", "class MyImpl : Diamond {}"), 0);

    ASSERT_LT(mod->Build(), 0);

    EXPECT_THAT(
        messages,
        ElementsAre(
            "Missing implementation of 'double Diamond::getDouble(double)'",
            "Missing implementation of 'int8 Diamond::getChar()'",
            "Missing implementation of 'double Diamond::getDouble()'"
        )
    );
}
