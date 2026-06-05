#include <AngelScriptWrapper/Engine.hpp>
#include <AngelScriptWrapperTests/ScriptDebugging.hpp>
#include <AngelScriptWrapperTests/TestRefType.hpp>
#include <gtest/gtest.h>
#include <limits>
#include <scriptarray.h>
#include <scripthelper.h>
#include <scriptstdstring.h>

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

    static inline const double div(const double a, const double b) {
        return a / b;
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
    Global obj;

    constexpr auto intendedAdd = as::FindOverload<^^Global, const double, true, const double, const double>("add2");
    AS_NAMESPACE_QUALIFIER asDWORD callConv = -1;
    ASSERT_GE(engine.RegisterGlobalFunction<intendedAdd>(callConv, &obj), 0);
    ASSERT_EQ(callConv, AS_NAMESPACE_QUALIFIER asCALL_THISCALL_ASGLOBAL);

    double result = 0;
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return add2(5.5, 2.5);", &result, 11), 0);
    EXPECT_DOUBLE_EQ(result, 3.0);
}

TEST(AngelScriptEngineGlobalFunctions, SimpleStaticMethod) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());

    AS_NAMESPACE_QUALIFIER asDWORD callConv = -1;
    ASSERT_GE(engine.RegisterGlobalFunction<^^Global::div>(callConv), 0);
    ASSERT_EQ(callConv, AS_NAMESPACE_QUALIFIER asCALL_CDECL);

    double result = 0;
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return div(5, 10);", &result, 11), 0);
    EXPECT_DOUBLE_EQ(result, 0.5);
}

// TODO: working on Ubuntu WSL with 64-bit compiler, so can't/shouldn't test invocation of StdCall for the time being.

/* __attribute__((stdcall)) */ double square[[ = as::Name("s"), = as::StdCall ]](double a) {
    return a * a;
}

TEST(AngelScriptEngineGlobalFunctions, RenameStdCallFunction) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());

    AS_NAMESPACE_QUALIFIER asDWORD callConv = -1;
    ASSERT_GE(engine.RegisterGlobalFunction<^^square>(callConv), 0);
    ASSERT_EQ(callConv, AS_NAMESPACE_QUALIFIER asCALL_STDCALL);

    // double result = 0;
    // ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return s(7.8);", &result, 11), 0);
    // EXPECT_DOUBLE_EQ(result, 60.84);
}

double cSquare[[ = as::Name("s"), = as::CDecl ]](double a) {
    return a * a;
}

TEST(AngelScriptEngineGlobalFunctions, RenameCDeclFunction) {
    as::Engine<{ .CallConventionDefault = AS_NAMESPACE_QUALIFIER asCALL_STDCALL }> engine;
    ASSERT_TRUE(engine.HasEngine());

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

    AS_NAMESPACE_QUALIFIER asDWORD callConv = -1;
    ASSERT_GE(engine.RegisterGlobalFunction<^^bitwiseOrIn>(callConv), 0);
    ASSERT_EQ(callConv, AS_NAMESPACE_QUALIFIER asCALL_CDECL);

    std::uint8_t result = 0;
    ASSERT_GE(
        AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "uint8 r = 16; return bitwiseOrIn(4, 8, r);", &result, 6), 0
    );
    EXPECT_EQ(result, 28);
}

int const getRefCount_ref(as::TestRefType const& obj) {
    return obj.RefCount();
}

TEST(AngelScriptEngineGlobalFunctions, RefTypeParamByRef) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());
    ASSERT_GE(as::TestRefType::Register(engine.Ptr()), 0);

    AS_NAMESPACE_QUALIFIER asDWORD callConv = -1;
    ASSERT_GE(engine.RegisterGlobalFunction<^^getRefCount_ref>(callConv), 0);
    ASSERT_EQ(callConv, AS_NAMESPACE_QUALIFIER asCALL_CDECL);

    int count = 0;
    ASSERT_GE(
        AS_NAMESPACE_QUALIFIER ExecuteString(
            engine.Ptr(), "TestRefType a; getRefCount_ref(a); return a.RefCount();", &count, 4
        ),
        0
    );
    // Passing by ref will not increment the ref counter:
    EXPECT_EQ(count, 1);
}

int const getRefCount_ptr(as::TestRefType const* obj) {
    return obj->RefCount();
}

TEST(AngelScriptEngineGlobalFunctions, RefTypeParamByPtr) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());
    ASSERT_GE(as::TestRefType::Register(engine.Ptr()), 0);

    AS_NAMESPACE_QUALIFIER asDWORD callConv = -1;
    ASSERT_GE(engine.RegisterGlobalFunction<^^getRefCount_ptr>(callConv), 0);
    ASSERT_EQ(callConv, AS_NAMESPACE_QUALIFIER asCALL_CDECL);

    int count = 0;
    ASSERT_GE(
        AS_NAMESPACE_QUALIFIER ExecuteString(
            engine.Ptr(), "TestRefType a; getRefCount_ptr(a); return a.RefCount();", &count, 4
        ),
        0
    );
    // Passing by ptr will increment the ref counter, but non-auto handles will not decrement for us:
    EXPECT_EQ(count, 2);
}

int const getRefCount_autoPtr(as::TestRefType* obj[[= as::Auto]]) {
    return obj->RefCount();
}

TEST(AngelScriptEngineGlobalFunctions, RefTypeParamByAutoPtr) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());
    ASSERT_GE(as::TestRefType::Register(engine.Ptr()), 0);

    AS_NAMESPACE_QUALIFIER asDWORD callConv = -1;
    ASSERT_GE(engine.RegisterGlobalFunction<^^getRefCount_autoPtr>(callConv), 0);
    ASSERT_EQ(callConv, AS_NAMESPACE_QUALIFIER asCALL_CDECL);

    int count = 0;
    ASSERT_GE(
        AS_NAMESPACE_QUALIFIER ExecuteString(
            engine.Ptr(), "TestRefType a; getRefCount_autoPtr(a); return a.RefCount();", &count, 4
        ),
        0
    );
    // Passing by ptr will increment the ref counter, and auto handles will decrement for us:
    EXPECT_EQ(count, 1);
}

as::TestRefType* createObj() {
    return as::TestRefType::Factory();
}

TEST(AngelScriptEngineGlobalFunctions, RefTypeReturn) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());
    ASSERT_GE(as::TestRefType::Register(engine.Ptr()), 0);

    AS_NAMESPACE_QUALIFIER asDWORD callConv = -1;
    ASSERT_GE(engine.RegisterGlobalFunction<^^createObj>(callConv), 0);
    ASSERT_EQ(callConv, AS_NAMESPACE_QUALIFIER asCALL_CDECL);

    int count = 0;
    ASSERT_GE(
        AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "TestRefType@ a = createObj(); return a.RefCount();", &count, 4),
        0
    );
    // Returning non-auto handles will not increment the ref counter for us
    // (it is 1 because TestRefType initializes with 1 already):
    EXPECT_EQ(count, 1);
}

TEST(AngelScriptEngineGlobalFunctions, RefTypeAutoReturn) {
    as::Engine<{ .AutoHandleDefault = true }> engine;
    ASSERT_TRUE(engine.HasEngine());
    ASSERT_GE(as::TestRefType::Register(engine.Ptr()), 0);

    AS_NAMESPACE_QUALIFIER asDWORD callConv = -1;
    ASSERT_GE(engine.RegisterGlobalFunction<^^createObj>(callConv), 0);
    ASSERT_EQ(callConv, AS_NAMESPACE_QUALIFIER asCALL_CDECL);

    int count = 0;
    ASSERT_GE(
        AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "TestRefType@ a = createObj(); return a.RefCount();", &count, 4),
        0
    );
    // Returning auto handles will increment the ref counter for us
    // (it is 2 because TestRefType initializes with 1 already):
    EXPECT_EQ(count, 2);
}

as::TestRefType& retRef(as::TestRefType& o) {
    return o;
}

TEST(AngelScriptEngineGlobalFunctions, RefTypeRefReturn) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());
    ASSERT_GE(as::TestRefType::Register(engine.Ptr()), 0);

    AS_NAMESPACE_QUALIFIER asDWORD callConv = -1;
    ASSERT_GE(engine.RegisterGlobalFunction<^^retRef>(callConv), 0);
    ASSERT_EQ(callConv, AS_NAMESPACE_QUALIFIER asCALL_CDECL);

    int count = 0;
    ASSERT_GE(
        AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "TestRefType a; return retRef(a).RefCount();", &count, 4), 0
    );
    // Returning by reference also does not impact the reference counter:
    EXPECT_EQ(count, 1);
}

AS_NAMESPACE_QUALIFIER asUINT
getSize(const AS_NAMESPACE_QUALIFIER CScriptArray* const arr[[ = as::subtype::String, = as::Auto ]]) {
    return arr->GetSize();
}

TEST(AngelScriptEngineGlobalFunctions, TemplateParameter) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());
    AS_NAMESPACE_QUALIFIER RegisterStdString(engine.Ptr());
    AS_NAMESPACE_QUALIFIER RegisterScriptArray(engine.Ptr(), false);
    ASSERT_GE(as::TestRefType::Register(engine.Ptr()), 0);

    AS_NAMESPACE_QUALIFIER asDWORD callConv = -1;
    ASSERT_GE(engine.RegisterGlobalFunction<^^getSize>(callConv), 0);
    ASSERT_EQ(callConv, AS_NAMESPACE_QUALIFIER asCALL_CDECL);

    AS_NAMESPACE_QUALIFIER asUINT count = std::numeric_limits<AS_NAMESPACE_QUALIFIER asUINT>::max();
    ASSERT_GE(
        AS_NAMESPACE_QUALIFIER ExecuteString(
            engine.Ptr(), "array<string> a; a.insertLast(\"HI\"); return getSize(a);", &count, 8
        ),
        0
    );
    EXPECT_EQ(count, 1);

    // Doesn't work on non-string arrays:
    ASSERT_LT(
        AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "array<int> a; a.insertLast(5); return getSize(a);", &count, 8), 0
    );
}

const AS_NAMESPACE_QUALIFIER CScriptArray* get[[= as::subtype::Int32]](
    const AS_NAMESPACE_QUALIFIER CScriptArray* a[[= as::subtype::Int32]]
) {
    return a;
}

TEST(AngelScriptEngineGlobalFunctions, TemplateParameterAndReturn) {
    as::Engine<{ .AutoHandleDefault = true }> engine;
    ASSERT_TRUE(engine.HasEngine());
    AS_NAMESPACE_QUALIFIER RegisterScriptArray(engine.Ptr(), false);
    ASSERT_GE(as::TestRefType::Register(engine.Ptr()), 0);

    AS_NAMESPACE_QUALIFIER asDWORD callConv = -1;
    ASSERT_GE(engine.RegisterGlobalFunction<^^get>(callConv), 0);
    ASSERT_EQ(callConv, AS_NAMESPACE_QUALIFIER asCALL_CDECL);

    const auto typeId = engine.Ptr()->GetTypeIdByDecl("const array<int>@");
    const AS_NAMESPACE_QUALIFIER CScriptArray* a = nullptr;
    ASSERT_GE(
        AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "array<int> a; a.insertLast(8); return get(a);", &a, typeId), 0
    );
    ASSERT_TRUE(a);
    EXPECT_EQ(a->GetSize(), 1);
    a->Release();
}

void explicitGeneric[[= as::Generic("double mult(double a, double b)")]](AS_NAMESPACE_QUALIFIER asIScriptGeneric* s) {
    double a = s->GetArgDouble(0);
    double b = s->GetArgDouble(1);

    s->SetReturnDouble(a * b);
}

TEST(AngelScriptEngineGlobalFunctions, ExplicitGeneric) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());
    as::SetMessageCallback(engine);

    AS_NAMESPACE_QUALIFIER asDWORD callConv = -1;
    ASSERT_GE(engine.RegisterGlobalFunction<^^explicitGeneric>(callConv), 0);
    ASSERT_EQ(callConv, AS_NAMESPACE_QUALIFIER asCALL_GENERIC);

    double res = 0.0;
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return mult(5.6, 11.2);", &res, 11), 0);
    EXPECT_DOUBLE_EQ(res, 62.72);
}

double derivedGeneric[[= as::Generic()]](double a, double b) {
    return a * b;
}

TEST(AngelScriptEngineGlobalFunctions, DerivedGeneric) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());
    as::SetMessageCallback(engine);

    AS_NAMESPACE_QUALIFIER asDWORD callConv = -1;
    ASSERT_GE(engine.RegisterGlobalFunction<^^derivedGeneric>(callConv), 0);
    ASSERT_EQ(callConv, AS_NAMESPACE_QUALIFIER asCALL_GENERIC);

    double res = 0.0;
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return derivedGeneric(5.6, 11.2);", &res, 11), 0);
    EXPECT_DOUBLE_EQ(res, 62.72);
}

double derivedGenericNoAnno(double a, double b) {
    return a * b;
}

TEST(AngelScriptEngineGlobalFunctions, DerivedGenericNoAnno) {
    as::Engine<{ .CallConventionDefault = AS_NAMESPACE_QUALIFIER asCALL_GENERIC }> engine;
    ASSERT_TRUE(engine.HasEngine());
    as::SetMessageCallback(engine);

    AS_NAMESPACE_QUALIFIER asDWORD callConv = -1;
    ASSERT_GE(engine.RegisterGlobalFunction<^^derivedGenericNoAnno>(callConv), 0);
    ASSERT_EQ(callConv, AS_NAMESPACE_QUALIFIER asCALL_GENERIC);

    double res = 0.0;
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return derivedGenericNoAnno(5.6, 11.2);", &res, 11), 0);
    EXPECT_DOUBLE_EQ(res, 62.72);
}

struct Gen {
    static inline void explicitGeneric[[= as::Generic("double mult(double a, double b)")]](
        AS_NAMESPACE_QUALIFIER asIScriptGeneric* s
    ) {
        double a = s->GetArgDouble(0);
        double b = s->GetArgDouble(1);

        s->SetReturnDouble(a * b);
    }

    static inline double derivedGeneric[[= as::Generic()]](double a, double b) {
        return a * b;
    }

    static inline double derivedGenericNoAnno(double a, double b) {
        return a * b;
    }
};

TEST(AngelScriptEngineGlobalFunctions, ExplicitGenericStaticClassMember) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());
    as::SetMessageCallback(engine);

    AS_NAMESPACE_QUALIFIER asDWORD callConv = -1;
    ASSERT_GE(engine.RegisterGlobalFunction<^^Gen::explicitGeneric>(callConv), 0);
    ASSERT_EQ(callConv, AS_NAMESPACE_QUALIFIER asCALL_GENERIC);

    double res = 0.0;
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return mult(5.6, 11.2);", &res, 11), 0);
    EXPECT_DOUBLE_EQ(res, 62.72);
}

TEST(AngelScriptEngineGlobalFunctions, DerivedGenericStaticClassMember) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());
    as::SetMessageCallback(engine);

    AS_NAMESPACE_QUALIFIER asDWORD callConv = -1;
    ASSERT_GE(engine.RegisterGlobalFunction<^^Gen::derivedGeneric>(callConv), 0);
    ASSERT_EQ(callConv, AS_NAMESPACE_QUALIFIER asCALL_GENERIC);

    double res = 0.0;
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return derivedGeneric(5.6, 11.2);", &res, 11), 0);
    EXPECT_DOUBLE_EQ(res, 62.72);
}

TEST(AngelScriptEngineGlobalFunctions, DerivedGenericNoAnnoStaticClassMember) {
    as::Engine<{ .CallConventionDefault = AS_NAMESPACE_QUALIFIER asCALL_GENERIC }> engine;
    ASSERT_TRUE(engine.HasEngine());
    as::SetMessageCallback(engine);

    AS_NAMESPACE_QUALIFIER asDWORD callConv = -1;
    ASSERT_GE(engine.RegisterGlobalFunction<^^Gen::derivedGenericNoAnno>(callConv), 0);
    ASSERT_EQ(callConv, AS_NAMESPACE_QUALIFIER asCALL_GENERIC);

    double res = 0.0;
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return derivedGenericNoAnno(5.6, 11.2);", &res, 11), 0);
    EXPECT_DOUBLE_EQ(res, 62.72);
}
