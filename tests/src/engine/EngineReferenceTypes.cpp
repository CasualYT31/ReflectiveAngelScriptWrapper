#include <AngelScriptWrapper/Engine.hpp>
#include <AngelScriptWrapper/WeakReferenceType.hpp>
#include <AngelScriptWrapperTests/ScriptDebugging.hpp>
#include <AngelScriptWrapperTests/TestRefType.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <scripthelper.h>
#include <weakref.h>

using namespace testing;

int finalRefCountIncrease(const as::TestRefType* const obj) {
    return obj->RefCount();
}

TEST(AngelScriptEngineReferenceTypes, BasicType) {
    as::Engine<{ .AutoHandleDefault = true }> engine;
    ASSERT_TRUE(engine.HasEngine());

    const auto typeId = engine.RegisterObjectType<^^as::TestRefType>();
    const auto expectedTypeId = engine.Ptr()->GetTypeIdByDecl("TestRefType");
    EXPECT_EQ(typeId, expectedTypeId);

    EXPECT_TRUE(engine.HasRegisteredObjectType<as::TestRefType>());
    EXPECT_FALSE(engine.HasRegisteredObjectType<as::ReferenceType>());

    ASSERT_GE(engine.RegisterGlobalFunction<^^finalRefCountIncrease>(), 0);

    AS_NAMESPACE_QUALIFIER asIScriptModule* mod =
        engine.Ptr()->GetModule("test", AS_NAMESPACE_QUALIFIER asGM_ALWAYS_CREATE);
    ASSERT_TRUE(mod);

    ASSERT_GE(
        mod->AddScriptSection("test.as", R"(
            int refCountIncrease(TestRefType@ const obj) {
                return obj.RefCount();
            }

            int refCountIncreaseTwice(TestRefType@ const obj) {
                return finalRefCountIncrease(obj);
            }

            int main() {
                TestRefType obj;
                return refCountIncrease(obj) /* 2 */ + refCountIncreaseTwice(obj) /* 3 */;
            }
        )"),
        0
    );

    ASSERT_GE(mod->Build(), 0);

    AS_NAMESPACE_QUALIFIER asIScriptContext* ctx = engine.Ptr()->CreateContext();
    ASSERT_TRUE(ctx);

    AS_NAMESPACE_QUALIFIER asIScriptFunction* func = mod->GetFunctionByDecl("int main()");
    ASSERT_TRUE(func);

    ASSERT_GE(ctx->Prepare(func), 0);

    ASSERT_EQ(ctx->Execute(), AS_NAMESPACE_QUALIFIER asEXECUTION_FINISHED);

    const auto res = static_cast<int>(ctx->GetReturnDWord());
    EXPECT_EQ(res, 5);
}

struct[[= as::RefType]] TypeWithCustomFactory : public as::ReferenceType {
    TypeWithCustomFactory(int number) : m_number(number) {}

    virtual ~TypeWithCustomFactory() noexcept = default;

    static inline TypeWithCustomFactory* CustomFactory[
        [ = as::Behaviour(AS_NAMESPACE_QUALIFIER asBEHAVE_FACTORY), = as::NonAuto ]](int number) {
        return new TypeWithCustomFactory(number);
    }

    int getNumber() {
        return m_number;
    }

private:
    int m_number = 0;
};

TEST(AngelScriptEngineReferenceTypes, BasicTypeWithCustomFactory) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());

    std::vector<std::string> messages;
    ASSERT_GE(as::SetMessageCallback(engine, &messages), 0);

    const auto typeId = engine.RegisterObjectType<^^TypeWithCustomFactory>();
    const auto expectedTypeId = engine.Ptr()->GetTypeIdByDecl("TypeWithCustomFactory");
    EXPECT_EQ(typeId, expectedTypeId);

    ASSERT_LT(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "TypeWithCustomFactory obj;"), 0);
    EXPECT_THAT(messages, ElementsAre("No default constructor for object of type 'TypeWithCustomFactory'."));

    int ret = 0;
    ASSERT_GE(
        AS_NAMESPACE_QUALIFIER ExecuteString(
            engine.Ptr(), "TypeWithCustomFactory obj(5); return obj.getNumber();", &ret, 4
        ),
        0
    );
    EXPECT_EQ(ret, 5);
}

constexpr auto AuxFactory = as::AuxLabel("AuxFactory");

struct FactoryHelper {
    int num;
};

struct[[= as::RefType]] TypeWithAuxiliaryLastFactory : public as::ReferenceType {
    TypeWithAuxiliaryLastFactory(int number) : m_number(number) {}

    virtual ~TypeWithAuxiliaryLastFactory() noexcept = default;

    static inline TypeWithAuxiliaryLastFactory* CustomFactory[
        [ = as::Behaviour(AS_NAMESPACE_QUALIFIER asBEHAVE_FACTORY), = as::NonAuto, = as::ObjLast, = AuxFactory ]](
        int ignored[[= as::DefVal("0")]], FactoryHelper* const helper
    ) {
        return new TypeWithAuxiliaryLastFactory(helper->num);
    }

    int getNumber() {
        return m_number;
    }

private:
    int m_number = 0;
};

TEST(AngelScriptEngineReferenceTypes, BasicTypeWithAuxiliaryLastFactory) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());

    auto helper = FactoryHelper(5);

    engine.AddAuxiliaryObject(AuxFactory, &helper);

    const auto typeId = engine.RegisterObjectType<^^TypeWithAuxiliaryLastFactory>();

    const auto expectedTypeId = engine.Ptr()->GetTypeIdByDecl("TypeWithAuxiliaryLastFactory");
    EXPECT_EQ(typeId, expectedTypeId);

    int ret = 0;
    ASSERT_GE(
        AS_NAMESPACE_QUALIFIER ExecuteString(
            engine.Ptr(), "TypeWithAuxiliaryLastFactory obj; return obj.getNumber();", &ret, 4
        ),
        0
    );
    EXPECT_EQ(ret, 5);
}

struct[[= as::RefType]] TypeWithAuxiliaryFirstFactory : public as::ReferenceType {
    TypeWithAuxiliaryFirstFactory(int number) : m_number(number) {}

    virtual ~TypeWithAuxiliaryFirstFactory() noexcept = default;

    static inline TypeWithAuxiliaryFirstFactory* CustomFactory[
        [ = as::Behaviour(AS_NAMESPACE_QUALIFIER asBEHAVE_FACTORY), = as::NonAuto, = as::ObjFirst, = AuxFactory ]](
        FactoryHelper* const helper, int ignored[[= as::DefVal("0")]]
    ) {
        return new TypeWithAuxiliaryFirstFactory(helper->num);
    }

    int getNumber() {
        return m_number;
    }

private:
    int m_number = 0;
};

TEST(AngelScriptEngineReferenceTypes, BasicTypeWithAuxiliaryFirstFactory) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());

    auto helper = FactoryHelper(5);

    engine.AddAuxiliaryObject(AuxFactory, &helper);

    const auto typeId = engine.RegisterObjectType<^^TypeWithAuxiliaryFirstFactory>();

    const auto expectedTypeId = engine.Ptr()->GetTypeIdByDecl("TypeWithAuxiliaryFirstFactory");
    EXPECT_EQ(typeId, expectedTypeId);

    int ret = 0;
    ASSERT_GE(
        AS_NAMESPACE_QUALIFIER ExecuteString(
            engine.Ptr(), "TypeWithAuxiliaryFirstFactory obj; return obj.getNumber();", &ret, 4
        ),
        0
    );
    EXPECT_EQ(ret, 5);
}

struct[[= as::RefType]] TypeWithoutRefCounting {
    TypeWithoutRefCounting(int number) : m_number(number) {}

    static inline TypeWithoutRefCounting* Factory[
        [ = as::Behaviour(AS_NAMESPACE_QUALIFIER asBEHAVE_FACTORY), = as::NonAuto ]]() {
        return new TypeWithoutRefCounting(0);
    }

    int getNumber() const {
        return m_number;
    }

private:
    int m_number = 0;
};

TEST(AngelScriptEngineReferenceTypes, BasicTypeWithoutRefCounting) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());

    const auto typeId = engine.RegisterObjectType<^^TypeWithoutRefCounting>();
    const auto expectedTypeId = engine.Ptr()->GetTypeIdByDecl("TypeWithoutRefCounting");
    EXPECT_EQ(typeId, expectedTypeId);

    TypeWithoutRefCounting obj(5);

    AS_NAMESPACE_QUALIFIER asIScriptModule* mod =
        engine.Ptr()->GetModule("test", AS_NAMESPACE_QUALIFIER asGM_ALWAYS_CREATE);
    ASSERT_TRUE(mod);

    ASSERT_GE(
        mod->AddScriptSection("test.as", "int main(const TypeWithoutRefCounting& test) { return test.getNumber(); }"), 0
    );

    ASSERT_GE(mod->Build(), 0);

    AS_NAMESPACE_QUALIFIER asIScriptContext* ctx = engine.Ptr()->CreateContext();
    ASSERT_TRUE(ctx);

    AS_NAMESPACE_QUALIFIER asIScriptFunction* func = mod->GetFunctionByDecl("int main(const TypeWithoutRefCounting&)");
    ASSERT_TRUE(func);

    ASSERT_GE(ctx->Prepare(func), 0);

    ASSERT_GE(ctx->SetArgObject(0, &obj), 0);

    ASSERT_EQ(ctx->Execute(), AS_NAMESPACE_QUALIFIER asEXECUTION_FINISHED);

    const auto res = static_cast<int>(ctx->GetReturnDWord());
    EXPECT_EQ(res, 5);
}

struct[[= as::RefType]] SingleReferenceType {
    SingleReferenceType(int number) : m_number(number) {}

    int getNumber() const {
        return m_number;
    }

private:
    int m_number = 0;
};

TEST(AngelScriptEngineReferenceTypes, SingleReferenceType) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());

    std::vector<std::string> messages;
    ASSERT_GE(as::SetMessageCallback(engine, &messages), 0);

    const auto typeId = engine.RegisterObjectType<^^SingleReferenceType>();
    const auto expectedTypeId = engine.Ptr()->GetTypeIdByDecl("SingleReferenceType");
    EXPECT_EQ(typeId, expectedTypeId);

    SingleReferenceType obj(5);
    ASSERT_GE(engine.RegisterGlobalProperty<^^obj>(&obj), 0);

    AS_NAMESPACE_QUALIFIER asIScriptModule* mod =
        engine.Ptr()->GetModule("test", AS_NAMESPACE_QUALIFIER asGM_ALWAYS_CREATE);
    ASSERT_TRUE(mod);

    ASSERT_GE(mod->AddScriptSection("test.as", "int main() { return obj.getNumber(); }"), 0);

    ASSERT_GE(mod->Build(), 0);

    AS_NAMESPACE_QUALIFIER asIScriptContext* ctx = engine.Ptr()->CreateContext();
    ASSERT_TRUE(ctx);

    AS_NAMESPACE_QUALIFIER asIScriptFunction* func = mod->GetFunctionByDecl("int main()");
    ASSERT_TRUE(func);

    ASSERT_GE(ctx->Prepare(func), 0);

    ASSERT_EQ(ctx->Execute(), AS_NAMESPACE_QUALIFIER asEXECUTION_FINISHED);

    const auto res = static_cast<int>(ctx->GetReturnDWord());
    EXPECT_EQ(res, 5);

    ASSERT_GE(engine.Ptr()->DiscardModule("test"), 0);

    mod = engine.Ptr()->GetModule("test", AS_NAMESPACE_QUALIFIER asGM_ALWAYS_CREATE);
    ASSERT_TRUE(mod);

    ASSERT_GE(mod->AddScriptSection("test.as", "int main() { SingleReferenceType myObj; }"), 0);

    ASSERT_LT(mod->Build(), 0);

    EXPECT_THAT(messages, ElementsAre("Compiling int main()", "Data type can't be 'SingleReferenceType'"));
}

struct[[= as::RefType]] PropertyAccessors : public as::TestRefType {
    static inline PropertyAccessors* Factory[
        [ = as::Behaviour(AS_NAMESPACE_QUALIFIER asBEHAVE_FACTORY), = as::NonAuto ]]() {
        return new PropertyAccessors();
    }

    inline double get_myNumber[[= as::PropertyAccessor]]() const {
        return m_number;
    }

    inline void set_myNumber[[= as::PropertyAccessor]](double newNumber) {
        m_number = newNumber;
    }

    inline double get_myIndexedNumber[[= as::PropertyAccessor]](unsigned int myIndex) const {
        return m_number * myIndex;
    }

    inline void set_myIndexedNumber[[= as::PropertyAccessor]](unsigned int myIndex, double newNumber) {
        m_number = newNumber * myIndex;
    }

private:
    double m_number = 0.0;
};

TEST(AngelScriptEngineReferenceTypes, RefTypeWithPropertyAccessors) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());

    const auto typeId = engine.RegisterObjectType<^^PropertyAccessors>();

    const auto expectedTypeId = engine.Ptr()->GetTypeIdByDecl("PropertyAccessors");
    EXPECT_EQ(typeId, expectedTypeId);

    double ret = 0;
    ASSERT_GE(
        AS_NAMESPACE_QUALIFIER ExecuteString(
            engine.Ptr(), "PropertyAccessors obj; obj.myNumber = 11.5; return obj.myNumber;", &ret, 11
        ),
        0
    );
    EXPECT_DOUBLE_EQ(ret, 11.5);

    ASSERT_GE(
        AS_NAMESPACE_QUALIFIER ExecuteString(
            engine.Ptr(),
            "PropertyAccessors obj; obj.myIndexedNumber[5] = 11.5; return obj.myIndexedNumber[2];",
            &ret,
            11
        ),
        0
    );
    EXPECT_DOUBLE_EQ(ret, 115.0);
}

struct[[= as::RefType]] Properties : public as::TestRefType {
    static inline Properties* Factory[[ = as::Behaviour(AS_NAMESPACE_QUALIFIER asBEHAVE_FACTORY), = as::NonAuto ]]() {
        const auto newObj = new Properties();
        newObj->indirect = new std::uint64_t(0);
        return newObj;
    }

    static inline Properties* Factory[[ = as::Behaviour(AS_NAMESPACE_QUALIFIER asBEHAVE_FACTORY), = as::NonAuto ]](
        Properties const& cpy
    ) {
        const auto newObj = new Properties();
        newObj->number = cpy.number;
        newObj->indirect = new std::uint64_t(0);
        if (cpy.indirect) { *newObj->indirect = *cpy.indirect; }
        return newObj;
    }

    virtual ~Properties() {
        if (indirect) { delete indirect; }
    }

    double number = 0;

    std::uint64_t* indirect;
};

TEST(AngelScriptEngineReferenceTypes, RefTypeWithProperties) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());

    const auto typeId = engine.RegisterObjectType<^^Properties>();

    const auto expectedTypeId = engine.Ptr()->GetTypeIdByDecl("Properties");
    EXPECT_EQ(typeId, expectedTypeId);

    double ret = 0;
    ASSERT_GE(
        AS_NAMESPACE_QUALIFIER ExecuteString(
            engine.Ptr(), "Properties obj; obj.number = 11.5; return obj.number;", &ret, 11
        ),
        0
    );
    EXPECT_DOUBLE_EQ(ret, 11.5);

    AS_NAMESPACE_QUALIFIER asIScriptModule* mod =
        engine.Ptr()->GetModule("test", AS_NAMESPACE_QUALIFIER asGM_ALWAYS_CREATE);
    ASSERT_TRUE(mod);

    ASSERT_GE(
        mod->AddScriptSection("test.as", "Properties main() { Properties obj; obj.number = 11.5; return obj; }"), 0
    );

    ASSERT_GE(mod->Build(), 0);

    AS_NAMESPACE_QUALIFIER asIScriptContext* ctx = engine.Ptr()->CreateContext();
    ASSERT_TRUE(ctx);

    AS_NAMESPACE_QUALIFIER asIScriptFunction* func = mod->GetFunctionByDecl("Properties main()");
    ASSERT_TRUE(func);

    ASSERT_GE(ctx->Prepare(func), 0);

    ASSERT_EQ(ctx->Execute(), AS_NAMESPACE_QUALIFIER asEXECUTION_FINISHED);

    Properties* res = nullptr;
    ASSERT_TRUE(res = static_cast<Properties*>(ctx->GetReturnObject()));

    EXPECT_DOUBLE_EQ(res->number, 11.5);

    std::uint64_t indirectRet = 0;
    ASSERT_GE(
        AS_NAMESPACE_QUALIFIER ExecuteString(
            engine.Ptr(), "Properties obj; obj.indirect = 115; return obj.indirect;", &indirectRet, 9
        ),
        0
    );
    EXPECT_EQ(indirectRet, 115);
}

struct[[= as::RefType]] PropertiesChild : public Properties {
    static inline PropertiesChild* Factory[[ = as::Behaviour(AS_NAMESPACE_QUALIFIER asBEHAVE_FACTORY), = as::NonAuto ]](
    ) {
        return new PropertiesChild();
    }
};

TEST(AngelScriptEngineReferenceTypes, RefTypeHierarchies) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());

    ASSERT_GE(engine.RegisterObjectType<^^PropertiesChild>(), 0);

    // Grandchild -> grandparent:
    int ret = 0;
    ASSERT_GE(
        AS_NAMESPACE_QUALIFIER ExecuteString(
            engine.Ptr(), "PropertiesChild obj; TestRefType@ casted = obj; return casted.RefCount();", &ret, 4
        ),
        0
    );
    EXPECT_EQ(ret, 2);

    // Grandchild -> parent:
    ASSERT_GE(
        AS_NAMESPACE_QUALIFIER ExecuteString(
            engine.Ptr(), "PropertiesChild obj; Properties@ casted = obj; return casted.RefCount();", &ret, 4
        ),
        0
    );
    EXPECT_EQ(ret, 2);

    // Parent -> grandparent:
    ASSERT_GE(
        AS_NAMESPACE_QUALIFIER ExecuteString(
            engine.Ptr(), "Properties obj; TestRefType@ casted = obj; return casted.RefCount();", &ret, 4
        ),
        0
    );
    EXPECT_EQ(ret, 2);

    // Parent -> grandchild (invalid):
    ASSERT_GE(
        AS_NAMESPACE_QUALIFIER ExecuteString(
            engine.Ptr(),
            "Properties obj; PropertiesChild@ casted = cast<PropertiesChild>(obj);"
            "if (casted is null) { return 0; } else { return 1; }",
            &ret,
            4
        ),
        0
    );
    EXPECT_EQ(ret, 0);

    // Parent -> grandchild (valid):
    ASSERT_GE(
        AS_NAMESPACE_QUALIFIER ExecuteString(
            engine.Ptr(),
            "PropertiesChild obj; Properties@ casted = obj;"
            "PropertiesChild@ finalCasted = cast<PropertiesChild>(casted);"
            "return finalCasted.RefCount();",
            &ret,
            4
        ),
        0
    );
    EXPECT_EQ(ret, 3);

    // Grandparent -> grandchild (invalid):
    ASSERT_GE(
        AS_NAMESPACE_QUALIFIER ExecuteString(
            engine.Ptr(),
            "TestRefType obj; PropertiesChild@ casted = cast<PropertiesChild>(obj);"
            "if (casted is null) { return 0; } else { return 1; }",
            &ret,
            4
        ),
        0
    );
    EXPECT_EQ(ret, 0);

    // Grandparent -> grandchild (valid):
    ASSERT_GE(
        AS_NAMESPACE_QUALIFIER ExecuteString(
            engine.Ptr(),
            "PropertiesChild obj; TestRefType@ casted = obj;"
            "PropertiesChild@ finalCasted = cast<PropertiesChild>(casted);"
            "return finalCasted.RefCount();",
            &ret,
            4
        ),
        0
    );
    EXPECT_EQ(ret, 3);

    // Grandparent -> parent (invalid):
    ASSERT_GE(
        AS_NAMESPACE_QUALIFIER ExecuteString(
            engine.Ptr(),
            "TestRefType obj; Properties@ casted = cast<Properties>(obj);"
            "if (casted is null) { return 0; } else { return 1; }",
            &ret,
            4
        ),
        0
    );
    EXPECT_EQ(ret, 0);

    // Grandparent -> parent (valid):
    ASSERT_GE(
        AS_NAMESPACE_QUALIFIER ExecuteString(
            engine.Ptr(),
            "Properties obj; TestRefType@ casted = obj;"
            "Properties@ finalCasted = cast<Properties>(casted);"
            "return finalCasted.RefCount();",
            &ret,
            4
        ),
        0
    );
    EXPECT_EQ(ret, 3);
}

TEST(AngelScriptEngineReferenceTypes, RefTypeHierarchiesAreNotRegisteredIfRecursionIsOff) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());

    // If relationships _are_ registered, this will return an error code, as the casting operators registered will try
    // to reference non-existant base types.
    ASSERT_GE((engine.RegisterObjectType<^^PropertiesChild, false>()), 0);

    EXPECT_TRUE(engine.HasRegisteredObjectType<PropertiesChild>());
    EXPECT_FALSE(engine.HasRegisteredObjectType<Properties>());
    EXPECT_FALSE(engine.HasRegisteredObjectType<as::TestRefType>());
}

struct[[= as::RefType]] TopLeft : virtual as::ReferenceType {
    virtual ~TopLeft() noexcept = default;

    static inline TopLeft* Factory[[ = as::Behaviour(AS_NAMESPACE_QUALIFIER asBEHAVE_FACTORY), = as::NonAuto ]]() {
        return new TopLeft();
    }

    /**
     * Reminder that AngelScript does not support virtual inheritance.
     * As a workaround, you'll need to explicitly redefine any virtual methods yourself.
     */
    virtual inline void AddRef[[= as::Behaviour(AS_NAMESPACE_QUALIFIER asBEHAVE_ADDREF)]]() const noexcept {
        as::ReferenceType::AddRef();
    }

    virtual inline void Release[[= as::Behaviour(AS_NAMESPACE_QUALIFIER asBEHAVE_RELEASE)]]() const noexcept {
        as::ReferenceType::Release();
    }

    virtual int identity() const {
        return 0;
    }
};

struct[[= as::RefType]] TopRight : virtual as::ReferenceType {
    virtual ~TopRight() noexcept = default;

    static inline TopRight* Factory[[ = as::Behaviour(AS_NAMESPACE_QUALIFIER asBEHAVE_FACTORY), = as::NonAuto ]]() {
        return new TopRight();
    }

    virtual inline void AddRef[[= as::Behaviour(AS_NAMESPACE_QUALIFIER asBEHAVE_ADDREF)]]() const noexcept {
        as::ReferenceType::AddRef();
    }

    virtual inline void Release[[= as::Behaviour(AS_NAMESPACE_QUALIFIER asBEHAVE_RELEASE)]]() const noexcept {
        as::ReferenceType::Release();
    }

    virtual int identity() const {
        return 1;
    }
};

struct[[= as::RefType]] MiddleLeft : virtual TopLeft {
    virtual ~MiddleLeft() noexcept = default;

    static inline MiddleLeft* Factory[[ = as::Behaviour(AS_NAMESPACE_QUALIFIER asBEHAVE_FACTORY), = as::NonAuto ]]() {
        return new MiddleLeft();
    }

    virtual inline void AddRef[[= as::Behaviour(AS_NAMESPACE_QUALIFIER asBEHAVE_ADDREF)]]() const noexcept {
        as::ReferenceType::AddRef();
    }

    virtual inline void Release[[= as::Behaviour(AS_NAMESPACE_QUALIFIER asBEHAVE_RELEASE)]]() const noexcept {
        as::ReferenceType::Release();
    }

    virtual int identity() const override {
        return 2;
    }
};

struct[[= as::RefType]] Middle : virtual TopLeft, virtual TopRight {
    virtual ~Middle() noexcept = default;

    static inline Middle* Factory[[ = as::Behaviour(AS_NAMESPACE_QUALIFIER asBEHAVE_FACTORY), = as::NonAuto ]]() {
        return new Middle();
    }

    virtual inline void AddRef[[= as::Behaviour(AS_NAMESPACE_QUALIFIER asBEHAVE_ADDREF)]]() const noexcept {
        as::ReferenceType::AddRef();
    }

    virtual inline void Release[[= as::Behaviour(AS_NAMESPACE_QUALIFIER asBEHAVE_RELEASE)]]() const noexcept {
        as::ReferenceType::Release();
    }

    virtual int identity() const override {
        return 3;
    }
};

struct[[= as::RefType]] MiddleRight : virtual TopRight {
    virtual ~MiddleRight() noexcept = default;

    static inline MiddleRight* Factory[[ = as::Behaviour(AS_NAMESPACE_QUALIFIER asBEHAVE_FACTORY), = as::NonAuto ]]() {
        return new MiddleRight();
    }

    virtual inline void AddRef[[= as::Behaviour(AS_NAMESPACE_QUALIFIER asBEHAVE_ADDREF)]]() const noexcept {
        as::ReferenceType::AddRef();
    }

    virtual inline void Release[[= as::Behaviour(AS_NAMESPACE_QUALIFIER asBEHAVE_RELEASE)]]() const noexcept {
        as::ReferenceType::Release();
    }

    virtual int identity() const override {
        return 4;
    }
};

struct[[= as::RefType]] BottomLeft : MiddleLeft, Middle {
    virtual ~BottomLeft() noexcept = default;

    static inline BottomLeft* Factory[[ = as::Behaviour(AS_NAMESPACE_QUALIFIER asBEHAVE_FACTORY), = as::NonAuto ]]() {
        return new BottomLeft();
    }

    virtual inline void AddRef[[= as::Behaviour(AS_NAMESPACE_QUALIFIER asBEHAVE_ADDREF)]]() const noexcept {
        as::ReferenceType::AddRef();
    }

    virtual inline void Release[[= as::Behaviour(AS_NAMESPACE_QUALIFIER asBEHAVE_RELEASE)]]() const noexcept {
        as::ReferenceType::Release();
    }

    int identity() const override final {
        return 5;
    }
};

struct[[= as::RefType]] BottomRight : Middle, MiddleRight {
    virtual ~BottomRight() noexcept = default;

    static inline BottomRight* Factory[[ = as::Behaviour(AS_NAMESPACE_QUALIFIER asBEHAVE_FACTORY), = as::NonAuto ]]() {
        return new BottomRight();
    }

    virtual inline void AddRef[[= as::Behaviour(AS_NAMESPACE_QUALIFIER asBEHAVE_ADDREF)]]() const noexcept {
        as::ReferenceType::AddRef();
    }

    virtual inline void Release[[= as::Behaviour(AS_NAMESPACE_QUALIFIER asBEHAVE_RELEASE)]]() const noexcept {
        as::ReferenceType::Release();
    }

    int identity() const override final {
        return 6;
    }
};

TEST(AngelScriptEngineReferenceTypes, RefTypeMultipleInheritance) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());

    ASSERT_GE(engine.RegisterObjectType<^^BottomLeft>(), 0);
    ASSERT_GE(engine.RegisterObjectType<^^BottomRight>(), 0);

    // clang-format off
    int ret = 0;
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "TopLeft obj; return obj.identity();", &ret, 4), 0);
    EXPECT_EQ(ret, 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "TopRight obj; return obj.identity();", &ret, 4), 0);
    EXPECT_EQ(ret, 1);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "MiddleLeft obj; return obj.identity();", &ret, 4), 0);
    EXPECT_EQ(ret, 2);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "Middle obj; return obj.identity();", &ret, 4), 0);
    EXPECT_EQ(ret, 3);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "MiddleRight obj; return obj.identity();", &ret, 4), 0);
    EXPECT_EQ(ret, 4);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "BottomLeft obj; return obj.identity();", &ret, 4), 0);
    EXPECT_EQ(ret, 5);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "BottomRight obj; return obj.identity();", &ret, 4), 0);
    EXPECT_EQ(ret, 6);
    // clang-format on

    ASSERT_GE(
        AS_NAMESPACE_QUALIFIER ExecuteString(
            engine.Ptr(), "BottomLeft obj; TopLeft@ casted = obj; return casted.identity();", &ret, 4
        ),
        0
    );
    EXPECT_EQ(ret, 5);

    ASSERT_GE(
        AS_NAMESPACE_QUALIFIER ExecuteString(
            engine.Ptr(), "BottomLeft obj; TopRight@ casted = obj; return casted.identity();", &ret, 4
        ),
        0
    );
    EXPECT_EQ(ret, 5);

    ASSERT_LT(
        AS_NAMESPACE_QUALIFIER ExecuteString(
            engine.Ptr(), "BottomLeft obj; MiddleRight@ casted = obj; return casted.identity();", &ret, 4
        ),
        0
    );

    ASSERT_GE(
        AS_NAMESPACE_QUALIFIER ExecuteString(
            engine.Ptr(),
            "BottomLeft obj; TopLeft@ casted = obj; MiddleLeft@ finalCast = cast<MiddleLeft>(casted);"
            "return finalCast.identity();",
            &ret,
            4
        ),
        0
    );
    EXPECT_EQ(ret, 5);
}

template <typename T> struct[[= as::RefType]] CopyableType : public T {
    static inline CopyableType<T>* Factory[[ = as::Behaviour(AS_NAMESPACE_QUALIFIER asBEHAVE_FACTORY), = as::NonAuto ]](
    ) {
        return new CopyableType<T>();
    }

    static inline CopyableType<T>* Factory[[ = as::Behaviour(AS_NAMESPACE_QUALIFIER asBEHAVE_FACTORY), = as::NonAuto ]](
        CopyableType<T> const& cpy, bool copyAssignment[[= as::DefVal("false")]] = false
    ) {
        if (copyAssignment) {
            auto newObj = new CopyableType<T>();
            *newObj = cpy;
            ++newObj->number;
            return newObj;
        } else {
            return new CopyableType<T>(cpy);
        }
    }

    int number = 0;
};

namespace as {
// This specialization assumes that you won't register more than one specialization of CopyableType in a single test.
template <typename T> constexpr std::string_view TypeName<CopyableType<T>> = "CopyableType";
} // namespace as

TEST(AngelScriptEngineReferenceTypes, CopyConstructionAndAssignment) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());

    ASSERT_GE(engine.RegisterObjectType<^^CopyableType<as::TestRefType>>(), 0);

    AS_NAMESPACE_QUALIFIER asIScriptModule* mod =
        engine.Ptr()->GetModule("test", AS_NAMESPACE_QUALIFIER asGM_ALWAYS_CREATE);
    ASSERT_TRUE(mod);

    ASSERT_GE(
        mod->AddScriptSection("test.as", R"(
        int incRef(const CopyableType@ const original, const bool copyType) {
            CopyableType copy(original, copyType);

            if (original.RefCount() != 2) {
                return 2;
            }
            if (copy.RefCount() != 1) {
                return 3;
            }
            if (copy.number != (copyType ? 11 : 10)) {
                return 4;
            }
            return 0;
        }

        int main(const bool copyType) {
            CopyableType original;
            original.number = 10;
            if (original.RefCount() != 1) {
                return 1;
            }

            return incRef(original, copyType);
        }
    )"),
        0
    );

    ASSERT_GE(mod->Build(), 0);

    AS_NAMESPACE_QUALIFIER asIScriptContext* ctx = engine.Ptr()->CreateContext();
    ASSERT_TRUE(ctx);

    AS_NAMESPACE_QUALIFIER asIScriptFunction* func = mod->GetFunctionByDecl("int main(const bool)");
    ASSERT_TRUE(func);

    ASSERT_GE(ctx->Prepare(func), 0);

    ASSERT_GE(ctx->SetArgByte(0, 0), 0);

    ASSERT_EQ(ctx->Execute(), AS_NAMESPACE_QUALIFIER asEXECUTION_FINISHED);

    auto res = static_cast<int>(ctx->GetReturnDWord());
    EXPECT_EQ(res, 0);

    ASSERT_GE(ctx->Prepare(func), 0);

    ASSERT_GE(ctx->SetArgByte(0, 255), 0);

    ASSERT_EQ(ctx->Execute(), AS_NAMESPACE_QUALIFIER asEXECUTION_FINISHED);

    res = static_cast<int>(ctx->GetReturnDWord());
    EXPECT_EQ(res, 0);
}

TEST(AngelScriptEngineReferenceTypes, MoveConstructionAndAssignment) {
    CopyableType<as::TestRefType> one;
    EXPECT_EQ(one.RefCount(), 1);

    {
        CopyableType<as::TestRefType> two;
        two.number = 10;
        EXPECT_EQ(two.RefCount(), 1);

        EXPECT_EQ(one.number, 0);
        one = std::move(two);
        EXPECT_EQ(one.number, 10);
        EXPECT_EQ(one.RefCount(), 1);
        EXPECT_EQ(two.RefCount(), 1);
    }

    EXPECT_EQ(one.RefCount(), 1);
    CopyableType<as::TestRefType> two(std::move(one));
    EXPECT_EQ(two.RefCount(), 1);
    EXPECT_EQ(two.number, 10);
    EXPECT_EQ(one.RefCount(), 1);
}

struct[[= as::RefType]] Global {
    void storeGlobalCopy(CopyableType<as::TestRefType>* handle[[= as::Auto]]) {
        copyWithSharedObj = handle;
    }

    void moveIntoGlobal(CopyableType<as::TestRefType>* handle) {
        copyWithOwnedObj = handle;
    }

    as::SharedObject<CopyableType<as::TestRefType>> copyWithSharedObj[[= as::DoNotRegister]];

    as::OwnedObject<CopyableType<as::TestRefType>> copyWithOwnedObj[[= as::DoNotRegister]];
};

TEST(AngelScriptEngineReferenceTypes, ReferenceTypeObjectWrapperInteraction) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());

    ASSERT_GE(engine.RegisterObjectType<^^CopyableType<as::TestRefType>>(), 0);
    ASSERT_GE(engine.RegisterObjectType<^^Global>(), 0);

    Global glob;
    ASSERT_GE(engine.RegisterGlobalProperty<^^glob>(&glob), 0);

    ASSERT_GE(
        AS_NAMESPACE_QUALIFIER ExecuteString(
            engine.Ptr(), "CopyableType myObj; myObj.number = 5; glob.storeGlobalCopy(myObj);"
        ),
        0
    );

    ASSERT_GE(
        AS_NAMESPACE_QUALIFIER ExecuteString(
            engine.Ptr(), "CopyableType myObj; myObj.number = 15; glob.moveIntoGlobal(myObj);"
        ),
        0
    );

    ASSERT_TRUE(glob.copyWithSharedObj);
    ASSERT_TRUE(glob.copyWithOwnedObj);

    EXPECT_EQ(glob.copyWithSharedObj->number, 5);
    EXPECT_EQ(glob.copyWithSharedObj->RefCount(), 1);

    EXPECT_EQ(glob.copyWithOwnedObj->number, 15);
    EXPECT_EQ(glob.copyWithOwnedObj->RefCount(), 1);
}

struct[[= as::RefType]] TestWeakRefType : public as::WeakReferenceType {
    static inline TestWeakRefType* Factory[[ = as::Behaviour(AS_NAMESPACE_QUALIFIER asBEHAVE_FACTORY), = as::NonAuto ]](
    ) {
        return new TestWeakRefType();
    }

    inline void nonConstOperation() {}
};

TEST(AngelScriptEngineReferenceTypes, WeakReferenceTypesInScripts) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());

    std::vector<std::string> messages;
    ASSERT_GE(as::SetMessageCallback(engine, &messages), 0);

    AS_NAMESPACE_QUALIFIER RegisterScriptWeakRef(engine.Ptr());

    ASSERT_GE(engine.RegisterObjectType<^^as::TestRefType>(), 0);
    ASSERT_GE(engine.RegisterObjectType<^^TestWeakRefType>(), 0);

    AS_NAMESPACE_QUALIFIER asIScriptModule* mod =
        engine.Ptr()->GetModule("test", AS_NAMESPACE_QUALIFIER asGM_ALWAYS_CREATE);
    ASSERT_TRUE(mod);

    ASSERT_GE(
        mod->AddScriptSection("test.as", R"(
        weakref<TestWeakRefType> weakCopy;

        int weakRefAssignment() {
            TestWeakRefType obj;
            @weakCopy = obj;

            if (obj.RefCount() != 1) {
                return 2;
            }
            if (weakCopy.get().RefCount() != 2) {
                return 3;
            }
            if (weakCopy.get() !is obj) {
                return 4;
            }
            return 0;
        }

        int main() {
            const auto res = weakRefAssignment();

            if (res != 0) {
                return res;
            }

            return weakCopy.get() is null ? 0 : 1;
        }
    )"),
        0
    );

    ASSERT_GE(mod->Build(), 0);

    AS_NAMESPACE_QUALIFIER asIScriptContext* ctx = engine.Ptr()->CreateContext();
    ASSERT_TRUE(ctx);

    AS_NAMESPACE_QUALIFIER asIScriptFunction* func = mod->GetFunctionByDecl("int main()");
    ASSERT_TRUE(func);

    ASSERT_GE(ctx->Prepare(func), 0);

    ASSERT_EQ(ctx->Execute(), AS_NAMESPACE_QUALIFIER asEXECUTION_FINISHED);

    auto res = static_cast<int>(ctx->GetReturnDWord());
    EXPECT_EQ(res, 0);

    ASSERT_LT(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "weakref<TestRefType> illegalWeakref;"), 0);
    EXPECT_THAT(
        messages,
        ElementsAre(
            "The subtype doesn't support weak references",
            "Attempting to instantiate invalid template type 'weakref<TestRefType>'"
        )
    );
}

struct[[= as::RefType]] WeakrefContainer : public as::ReferenceType {
    static AS_NAMESPACE_QUALIFIER asITypeInfo* subTypeInfo;

    static inline WeakrefContainer* Factory[
        [ = as::Behaviour(AS_NAMESPACE_QUALIFIER asBEHAVE_FACTORY), = as::NonAuto ]]() {
        return new WeakrefContainer();
    }

    inline WeakrefContainer() : weakRef(subTypeInfo) {}

    AS_NAMESPACE_QUALIFIER CScriptWeakRef weakRef[[= as::SubTypeList<const TestWeakRefType>()]];
};

AS_NAMESPACE_QUALIFIER asITypeInfo* WeakrefContainer::subTypeInfo = nullptr;

TEST(AngelScriptEngineReferenceTypes, WeakReferenceTypesInApplicationInterface) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());
    AS_NAMESPACE_QUALIFIER RegisterScriptWeakRef(engine.Ptr());

    ASSERT_GE(engine.RegisterObjectType<^^TestWeakRefType>(), 0);

    WeakrefContainer::subTypeInfo = engine.Ptr()->GetTypeInfoByDecl("weakref<const TestWeakRefType>");
    ASSERT_TRUE(WeakrefContainer::subTypeInfo);

    ASSERT_GE(engine.RegisterObjectType<^^WeakrefContainer>(), 0);

    AS_NAMESPACE_QUALIFIER asIScriptModule* mod =
        engine.Ptr()->GetModule("test", AS_NAMESPACE_QUALIFIER asGM_ALWAYS_CREATE);
    ASSERT_TRUE(mod);

    ASSERT_GE(
        mod->AddScriptSection("test.as", R"(
        WeakrefContainer weakCopy;

        int weakRefAssignment() {
            const TestWeakRefType obj;
            @weakCopy.weakRef = obj;

            if (obj.RefCount() != 1) {
                return 2;
            }
            if (weakCopy.weakRef.get().RefCount() != 2) {
                return 3;
            }
            if (weakCopy.weakRef.get() !is obj) {
                return 4;
            }
            return 0;
        }

        int main() {
            const auto res = weakRefAssignment();

            if (res != 0) {
                return res;
            }

            return weakCopy.weakRef.get() is null ? 0 : 1;
        }
    )"),
        0
    );

    ASSERT_GE(mod->Build(), 0);

    AS_NAMESPACE_QUALIFIER asIScriptContext* ctx = engine.Ptr()->CreateContext();
    ASSERT_TRUE(ctx);

    AS_NAMESPACE_QUALIFIER asIScriptFunction* func = mod->GetFunctionByDecl("int main()");
    ASSERT_TRUE(func);

    ASSERT_GE(ctx->Prepare(func), 0);

    ASSERT_EQ(ctx->Execute(), AS_NAMESPACE_QUALIFIER asEXECUTION_FINISHED);

    auto res = static_cast<int>(ctx->GetReturnDWord());
    EXPECT_EQ(res, 0);
}

TEST(AngelScriptEngineReferenceTypes, CopyConstructionAndAssignmentWeakReferences) {
    CopyableType<TestWeakRefType> one;
    one.number = 5;
    one.AddRef();
    one.GetWeakRefFlag()->Set(true);

    CopyableType<TestWeakRefType> two(one);
    ++two.number;

    CopyableType<TestWeakRefType> three;
    three.number = 3;
    three.AddRef();
    three.AddRef();
    three.GetWeakRefFlag()->Set(true);
    three = two;
    three.number -= 10;

    EXPECT_EQ(one.number, 5);
    EXPECT_EQ(two.number, 6);
    EXPECT_EQ(three.number, -4);
    EXPECT_EQ(one.RefCount(), 2);
    EXPECT_EQ(two.RefCount(), 1);
    EXPECT_EQ(three.RefCount(), 3);
    EXPECT_TRUE(one.GetWeakRefFlag()->Get());
    EXPECT_FALSE(two.GetWeakRefFlag()->Get());
    EXPECT_TRUE(three.GetWeakRefFlag()->Get());
}

TEST(AngelScriptEngineReferenceTypes, MoveConstructionAndAssignmentWeakReferences) {
    CopyableType<TestWeakRefType> one;
    one.number = 5;
    one.AddRef();
    one.GetWeakRefFlag()->Set(true);

    CopyableType<TestWeakRefType> two(std::move(one));
    ++two.number;

    CopyableType<TestWeakRefType> three;
    three.number = 3;
    three.AddRef();
    three.AddRef();
    three.GetWeakRefFlag()->Set(true);
    three = std::move(two);
    three.number -= 10;

    EXPECT_EQ(one.number, 5);
    EXPECT_EQ(two.number, 6);
    EXPECT_EQ(three.number, -4);
    EXPECT_EQ(one.RefCount(), 2);
    EXPECT_EQ(two.RefCount(), 1);
    EXPECT_EQ(three.RefCount(), 3);
    EXPECT_TRUE(one.GetWeakRefFlag()->Get());
    EXPECT_FALSE(two.GetWeakRefFlag()->Get());
    EXPECT_TRUE(three.GetWeakRefFlag()->Get());
}
