#include <angelscript.h>
#include <AngelScriptWrapper/Engine.hpp>
#include <AngelScriptWrapper/OwnedObject.hpp>
#include <AngelScriptWrapperTests/ScriptArray.hpp>
#include <AngelScriptWrapperTests/ScriptDebugging.hpp>
#include <AngelScriptWrapperTests/ScriptTestObject.hpp>
#include <cstdint>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <scripthelper.h>
#include <scriptstdstring.h>
#include <vector>

using namespace testing;

TEST(AngelScriptEngineGlobalProperties, NonConstPrimitiveValues) {
    std::int8_t int8 = -8, int8InScript;
    std::uint8_t uint8 = 8, uint8InScript;
    std::int16_t int16 = -16, int16InScript;
    std::uint16_t uint16 = 16, uint16InScript;
    std::int32_t int32 = -32, int32InScript;
    std::uint32_t uint32 = 32, uint32InScript;
    std::int64_t int64 = -64, int64InScript;
    std::uint64_t uint64 = 64, uint64InScript;
    float f = 3.14, fInScript;
    double d = 3.1415926536, dInScript;
    bool flag = false, flagInScript;

    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());

    // Register global properties.
    ASSERT_GE(engine.RegisterGlobalProperty("i8", &int8), 0);
    ASSERT_GE(engine.RegisterGlobalProperty("ui8", &uint8), 0);
    ASSERT_GE(engine.RegisterGlobalProperty("i16", &int16), 0);
    ASSERT_GE(engine.RegisterGlobalProperty("ui16", &uint16), 0);
    ASSERT_GE(engine.RegisterGlobalProperty("i32", &int32), 0);
    ASSERT_GE(engine.RegisterGlobalProperty("ui32", &uint32), 0);
    ASSERT_GE(engine.RegisterGlobalProperty("i64", &int64), 0);
    ASSERT_GE(engine.RegisterGlobalProperty("ui64", &uint64), 0);
    ASSERT_GE(engine.RegisterGlobalProperty("f", &f), 0);
    ASSERT_GE(engine.RegisterGlobalProperty("d", &d), 0);
    ASSERT_GE(engine.RegisterGlobalProperty("flag", &flag), 0);

    // Retrieve global properties within a script.
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return i8;", &int8InScript, 2), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return ui8;", &uint8InScript, 6), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return i16;", &int16InScript, 3), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return ui16;", &uint16InScript, 7), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return i32;", &int32InScript, 4), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return ui32;", &uint32InScript, 8), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return i64;", &int64InScript, 5), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return ui64;", &uint64InScript, 9), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return f;", &fInScript, 10), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return d;", &dInScript, 11), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return flag;", &flagInScript, 1), 0);

    // Are global properties registered correctly?
    EXPECT_EQ(int8InScript, int8);
    EXPECT_EQ(uint8InScript, uint8);
    EXPECT_EQ(int16InScript, int16);
    EXPECT_EQ(uint16InScript, uint16);
    EXPECT_EQ(int32InScript, int32);
    EXPECT_EQ(uint32InScript, uint32);
    EXPECT_EQ(int64InScript, int64);
    EXPECT_EQ(uint64InScript, uint64);
    EXPECT_FLOAT_EQ(fInScript, f);
    EXPECT_DOUBLE_EQ(dInScript, d);
    EXPECT_EQ(flagInScript, flag);

    // Update global properties within a script.
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "i8 += i8;"), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "ui8 += ui8;"), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "i16 += i16;"), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "ui16 += ui16;"), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "i32 += i32;"), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "ui32 += ui32;"), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "i64 += i64;"), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "ui64 += ui64;"), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "f += f;"), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "d += d;"), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "flag = !flag;"), 0);

    // Are global properties updated in C++?
    EXPECT_EQ(int8InScript * 2, int8);
    EXPECT_EQ(uint8InScript * 2, uint8);
    EXPECT_EQ(int16InScript * 2, int16);
    EXPECT_EQ(uint16InScript * 2, uint16);
    EXPECT_EQ(int32InScript * 2, int32);
    EXPECT_EQ(uint32InScript * 2, uint32);
    EXPECT_EQ(int64InScript * 2, int64);
    EXPECT_EQ(uint64InScript * 2, uint64);
    EXPECT_FLOAT_EQ(fInScript * 2.0, f);
    EXPECT_DOUBLE_EQ(dInScript * 2.0, d);
    EXPECT_EQ(!flagInScript, flag);

    // Updating global properties in C++ must update them in AngelScript.
    int8 -= int8;
    uint8 -= uint8;
    int16 -= int16;
    uint16 -= uint16;
    int32 -= int32;
    uint32 -= uint32;
    int64 -= int64;
    uint64 -= uint64;
    f -= f;
    d -= d;
    flag = !flag;

    // Retrieve global properties within a script.
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return i8;", &int8InScript, 2), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return ui8;", &uint8InScript, 6), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return i16;", &int16InScript, 3), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return ui16;", &uint16InScript, 7), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return i32;", &int32InScript, 4), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return ui32;", &uint32InScript, 8), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return i64;", &int64InScript, 5), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return ui64;", &uint64InScript, 9), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return f;", &fInScript, 10), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return d;", &dInScript, 11), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return flag;", &flagInScript, 1), 0);

    // Are changes in C++ reflected in AngelScript?
    EXPECT_EQ(int8InScript, int8);
    EXPECT_EQ(uint8InScript, uint8);
    EXPECT_EQ(int16InScript, int16);
    EXPECT_EQ(uint16InScript, uint16);
    EXPECT_EQ(int32InScript, int32);
    EXPECT_EQ(uint32InScript, uint32);
    EXPECT_EQ(int64InScript, int64);
    EXPECT_EQ(uint64InScript, uint64);
    EXPECT_FLOAT_EQ(fInScript, f);
    EXPECT_DOUBLE_EQ(dInScript, d);
    EXPECT_EQ(flagInScript, flag);
}

TEST(AngelScriptEngineGlobalProperties, ConstPrimitiveValues) {
    std::int8_t int8 = -8, int8InScript = 0;
    std::uint8_t uint8 = 8, uint8InScript = 0;
    std::int16_t int16 = -16, int16InScript = 0;
    std::uint16_t uint16 = 16, uint16InScript = 0;
    std::int32_t int32 = -32, int32InScript = 0;
    std::uint32_t uint32 = 32, uint32InScript = 0;
    std::int64_t int64 = -64, int64InScript = 0;
    std::uint64_t uint64 = 64, uint64InScript = 0;
    float f = 3.14, fInScript = 0.0;
    double d = 3.1415926536, dInScript = 0.0;
    bool flag = false, flagInScript = true;

    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());

    // Register global properties.
    ASSERT_GE(engine.RegisterGlobalProperty("i8", &int8, true), 0);
    ASSERT_GE(engine.RegisterGlobalProperty("ui8", &uint8, true), 0);
    ASSERT_GE(engine.RegisterGlobalProperty("i16", &int16, true), 0);
    ASSERT_GE(engine.RegisterGlobalProperty("ui16", &uint16, true), 0);
    ASSERT_GE(engine.RegisterGlobalProperty("i32", &int32, true), 0);
    ASSERT_GE(engine.RegisterGlobalProperty("ui32", &uint32, true), 0);
    ASSERT_GE(engine.RegisterGlobalProperty("i64", &int64, true), 0);
    ASSERT_GE(engine.RegisterGlobalProperty("ui64", &uint64, true), 0);
    ASSERT_GE(engine.RegisterGlobalProperty("f", &f, true), 0);
    ASSERT_GE(engine.RegisterGlobalProperty("d", &d, true), 0);
    ASSERT_GE(engine.RegisterGlobalProperty("flag", &flag, true), 0);

    // Retrieve global properties within a script.
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return i8;", &int8InScript, 2), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return ui8;", &uint8InScript, 6), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return i16;", &int16InScript, 3), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return ui16;", &uint16InScript, 7), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return i32;", &int32InScript, 4), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return ui32;", &uint32InScript, 8), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return i64;", &int64InScript, 5), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return ui64;", &uint64InScript, 9), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return f;", &fInScript, 10), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return d;", &dInScript, 11), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return flag;", &flagInScript, 1), 0);

    // Are global properties registered correctly?
    EXPECT_EQ(int8InScript, int8);
    EXPECT_EQ(uint8InScript, uint8);
    EXPECT_EQ(int16InScript, int16);
    EXPECT_EQ(uint16InScript, uint16);
    EXPECT_EQ(int32InScript, int32);
    EXPECT_EQ(uint32InScript, uint32);
    EXPECT_EQ(int64InScript, int64);
    EXPECT_EQ(uint64InScript, uint64);
    EXPECT_FLOAT_EQ(fInScript, f);
    EXPECT_DOUBLE_EQ(dInScript, d);
    EXPECT_EQ(flagInScript, flag);

    // You cannot update constant global properties within a script.
    ASSERT_LT(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "i8 += i8;"), 0);
    ASSERT_LT(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "ui8 += ui8;"), 0);
    ASSERT_LT(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "i16 += i16;"), 0);
    ASSERT_LT(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "ui16 += ui16;"), 0);
    ASSERT_LT(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "i32 += i32;"), 0);
    ASSERT_LT(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "ui32 += ui32;"), 0);
    ASSERT_LT(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "i64 += i64;"), 0);
    ASSERT_LT(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "ui64 += ui64;"), 0);
    ASSERT_LT(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "f += f;"), 0);
    ASSERT_LT(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "d += d;"), 0);
    ASSERT_LT(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "flag = !flag;"), 0);

    // Are constant global properties not updated in C++?
    EXPECT_EQ(int8InScript, int8);
    EXPECT_EQ(uint8InScript, uint8);
    EXPECT_EQ(int16InScript, int16);
    EXPECT_EQ(uint16InScript, uint16);
    EXPECT_EQ(int32InScript, int32);
    EXPECT_EQ(uint32InScript, uint32);
    EXPECT_EQ(int64InScript, int64);
    EXPECT_EQ(uint64InScript, uint64);
    EXPECT_FLOAT_EQ(fInScript, f);
    EXPECT_DOUBLE_EQ(dInScript, d);
    EXPECT_EQ(flagInScript, flag);

    // Updating global properties in C++ must update them in AngelScript.
    int8 -= int8;
    uint8 -= uint8;
    int16 -= int16;
    uint16 -= uint16;
    int32 -= int32;
    uint32 -= uint32;
    int64 -= int64;
    uint64 -= uint64;
    f -= f;
    d -= d;
    flag = !flag;

    // Retrieve global properties within a script.
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return i8;", &int8InScript, 2), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return ui8;", &uint8InScript, 6), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return i16;", &int16InScript, 3), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return ui16;", &uint16InScript, 7), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return i32;", &int32InScript, 4), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return ui32;", &uint32InScript, 8), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return i64;", &int64InScript, 5), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return ui64;", &uint64InScript, 9), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return f;", &fInScript, 10), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return d;", &dInScript, 11), 0);
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return flag;", &flagInScript, 1), 0);

    // Are changes in C++ reflected in AngelScript?
    EXPECT_EQ(int8InScript, int8);
    EXPECT_EQ(uint8InScript, uint8);
    EXPECT_EQ(int16InScript, int16);
    EXPECT_EQ(uint16InScript, uint16);
    EXPECT_EQ(int32InScript, int32);
    EXPECT_EQ(uint32InScript, uint32);
    EXPECT_EQ(int64InScript, int64);
    EXPECT_EQ(uint64InScript, uint64);
    EXPECT_FLOAT_EQ(fInScript, f);
    EXPECT_DOUBLE_EQ(dInScript, d);
    EXPECT_EQ(flagInScript, flag);
}

TEST(AngelScriptEngineGlobalProperties, NonConstStringValues) {
    std::string str = "Hello", strInScript;

    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());
    AS_NAMESPACE_QUALIFIER RegisterStdString(engine.Ptr());
    const auto stringTypeInfo = engine.Ptr()->GetTypeInfoByName("string");
    ASSERT_TRUE(stringTypeInfo);
    const auto stringTypeId = stringTypeInfo->GetTypeId();

    // Register global property.
    ASSERT_GE(engine.RegisterGlobalProperty("str", &str), 0);

    // Retrieve global property within a script.
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return str;", &strInScript, stringTypeId), 0);

    // Is global property registered correctly?
    EXPECT_EQ(strInScript, str);

    // Update global property within a script.
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "str += \", World!\";"), 0);

    // Is global property updated in C++?
    EXPECT_EQ("Hello, World!", str);

    // Updating global property in C++ must update it in AngelScript.
    str = "New";

    // Retrieve global property within a script.
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return str;", &strInScript, stringTypeId), 0);

    // Are changes in C++ reflected in AngelScript?
    EXPECT_EQ(strInScript, str);
}

TEST(AngelScriptEngineGlobalProperties, ConstStringValues) {
    std::string str = "Hello", strInScript;

    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());
    AS_NAMESPACE_QUALIFIER RegisterStdString(engine.Ptr());
    const auto stringTypeInfo = engine.Ptr()->GetTypeInfoByName("string");
    ASSERT_TRUE(stringTypeInfo);
    const auto stringTypeId = stringTypeInfo->GetTypeId();

    // Register global property.
    ASSERT_GE(engine.RegisterGlobalProperty("str", &str, true), 0);

    // Retrieve global property within a script.
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return str;", &strInScript, stringTypeId), 0);

    // Is global property registered correctly?
    EXPECT_EQ(strInScript, str);

    // You cannot update const global property within a script.
    ASSERT_LT(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "str += \", World!\";"), 0);

    // Is global property not updated in C++?
    EXPECT_EQ(strInScript, str);

    // Updating global property in C++ must update it in AngelScript.
    str = "New";

    // Retrieve global property within a script.
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return str;", &strInScript, stringTypeId), 0);

    // Are changes in C++ reflected in AngelScript?
    EXPECT_EQ(strInScript, str);
}

void messageCallback(AS_NAMESPACE_QUALIFIER asSMessageInfo* msg, std::vector<std::string>* state) {
    state->emplace_back(msg->message);
}

TEST(AngelScriptEngineGlobalProperties, NonConstReferenceTypes) {
    as::ScriptTestObject obj;
    int counterInScript = 0;
    obj.counter = 7;

    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());
    ASSERT_GT(as::ScriptTestObject::Register(engine.Ptr()), 0);

    // Register global property.
    ASSERT_GE(engine.RegisterGlobalProperty("obj", &obj), 0);

    // Global property is not a handle.
    std::vector<std::string> messages;
    ASSERT_GE(
        engine.Ptr()->SetMessageCallback(asFUNCTION(messageCallback), &messages, AS_NAMESPACE_QUALIFIER asCALL_CDECL), 0
    );
    ASSERT_LT(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "@obj = @obj;"), 0);
    ASSERT_THAT(messages, ElementsAre("Expression is not an l-value"));
    EXPECT_GE(engine.Ptr()->ClearMessageCallback(), 0);

    // Retrieve global property's property within a script.
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return obj.counter;", &counterInScript, 4), 0);

    // Is global property registered correctly?
    EXPECT_EQ(counterInScript, obj.counter);

    // Update global property within a script.
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "++obj.counter;"), 0);

    // Is global property updated in C++?
    EXPECT_EQ(8, obj.counter);

    // Updating global property in C++ must update it in AngelScript.
    obj.Release();

    // Retrieve global property within a script.
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return obj.Release();", &counterInScript, 4), 0);

    // Are changes in C++ reflected in AngelScript?
    EXPECT_EQ(counterInScript, 6);
}

TEST(AngelScriptEngineGlobalProperties, ConstReferenceTypes) {
    as::ScriptTestObject obj;
    int counterInScript = 0;
    obj.counter = 7;

    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());
    ASSERT_GT(as::ScriptTestObject::Register(engine.Ptr()), 0);

    // Register global property.
    ASSERT_GE(engine.RegisterGlobalProperty("obj", &obj, true), 0);

    // Global property is not a handle.
    std::vector<std::string> messages;
    ASSERT_GE(
        engine.Ptr()->SetMessageCallback(asFUNCTION(messageCallback), &messages, AS_NAMESPACE_QUALIFIER asCALL_CDECL), 0
    );
    ASSERT_LT(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "@obj = @obj;"), 0);
    ASSERT_THAT(messages, ElementsAre("Expression is not an l-value"));
    EXPECT_GE(engine.Ptr()->ClearMessageCallback(), 0);

    // Retrieve global property's property within a script.
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return obj.counter;", &counterInScript, 4), 0);

    // Is global property registered correctly?
    EXPECT_EQ(counterInScript, obj.counter);

    // Cannot update const global property within a script.
    ASSERT_LT(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "++obj.counter;"), 0);

    // Is global property not updated in C++?
    EXPECT_EQ(7, obj.counter);

    // Updating global property in C++ must update it in AngelScript.
    obj.Release();

    // Retrieve global property within a script (whilst performing const operation on it).
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return obj.Release();", &counterInScript, 4), 0);

    // Are changes in C++ reflected in AngelScript?
    EXPECT_EQ(counterInScript, 5);
}

TEST(AngelScriptEngineGlobalProperties, NonConstReferenceNonConstPointerTypes) {
    auto objOwner = std::make_unique<as::ScriptTestObject>();
    as::ScriptTestObject* obj = objOwner.get();
    int counterInScript = 0;
    obj->counter = 7;

    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());
    ASSERT_GT(as::ScriptTestObject::Register(engine.Ptr()), 0);

    // Register global property.
    ASSERT_GE(engine.RegisterGlobalProperty("obj", &obj), 0);

    // Global property is a non-const handle.
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "@obj = @obj;"), 0);

    // Retrieve global property's property within a script.
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return obj.counter;", &counterInScript, 4), 0);

    // Is global property registered correctly?
    EXPECT_EQ(counterInScript, obj->counter);

    // Update global property within a script.
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "++obj.counter;"), 0);

    // Is global property updated in C++?
    EXPECT_EQ(8, obj->counter);

    // Updating global property in C++ must update it in AngelScript.
    obj->Release();

    // Retrieve global property within a script.
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return obj.Release();", &counterInScript, 4), 0);

    // Are changes in C++ reflected in AngelScript?
    EXPECT_EQ(counterInScript, 6);
}

TEST(AngelScriptEngineGlobalProperties, ConstReferenceNonConstPointerTypes) {
    auto objOwner = std::make_unique<const as::ScriptTestObject>();
    const as::ScriptTestObject* obj = objOwner.get();
    int counterInScript = 0;
    obj->counter = 7;

    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());
    ASSERT_GT(as::ScriptTestObject::Register(engine.Ptr()), 0);

    // Register global property.
    ASSERT_GE(engine.RegisterGlobalProperty("obj", &obj), 0);

    // Global property is a non-const handle.
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "@obj = @obj;"), 0);

    // Retrieve global property's property within a script.
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return obj.counter;", &counterInScript, 4), 0);

    // Is global property registered correctly?
    EXPECT_EQ(counterInScript, obj->counter);

    // Cannot update const global property within a script.
    ASSERT_LT(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "++obj.counter;"), 0);

    // Is global property not updated in C++?
    EXPECT_EQ(7, obj->counter);

    // Updating global property in C++ must update it in AngelScript.
    obj->Release();

    // Retrieve global property's property within a script via const operation.
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return obj.Release();", &counterInScript, 4), 0);

    // Are changes in C++ reflected in AngelScript?
    EXPECT_EQ(counterInScript, 5);
}

TEST(AngelScriptEngineGlobalProperties, NonConstReferenceConstPointerTypes) {
    auto objOwner = std::make_unique<as::ScriptTestObject>();
    as::ScriptTestObject* obj = objOwner.get();
    int counterInScript = 0;
    obj->counter = 7;

    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());
    ASSERT_GT(as::ScriptTestObject::Register(engine.Ptr()), 0);

    // Register global property.
    ASSERT_GE(engine.RegisterGlobalProperty("obj", &obj, true), 0);

    // Global property is a const handle.
    std::vector<std::string> messages;
    ASSERT_GE(
        engine.Ptr()->SetMessageCallback(asFUNCTION(messageCallback), &messages, AS_NAMESPACE_QUALIFIER asCALL_CDECL), 0
    );
    ASSERT_LT(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "@obj = @obj;"), 0);
    ASSERT_THAT(messages, ElementsAre("Reference is read-only"));
    EXPECT_GE(engine.Ptr()->ClearMessageCallback(), 0);

    // Retrieve global property's property within a script.
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return obj.counter;", &counterInScript, 4), 0);

    // Is global property registered correctly?
    EXPECT_EQ(counterInScript, obj->counter);

    // Update global property within a script.
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "++obj.counter;"), 0);

    // Is global property updated in C++?
    EXPECT_EQ(8, obj->counter);

    // Updating global property in C++ must update it in AngelScript.
    obj->Release();

    // Retrieve global property within a script.
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return obj.Release();", &counterInScript, 4), 0);

    // Are changes in C++ reflected in AngelScript?
    EXPECT_EQ(counterInScript, 6);
}

TEST(AngelScriptEngineGlobalProperties, ConstReferenceConstPointerTypes) {
    auto objOwner = std::make_unique<as::ScriptTestObject>();
    const as::ScriptTestObject* obj = objOwner.get();
    int counterInScript = 0;
    obj->counter = 7;

    as::Engine engine;

    ASSERT_TRUE(engine.HasEngine());
    ASSERT_GT(as::ScriptTestObject::Register(engine.Ptr()), 0);

    // Register global property.
    ASSERT_GE(engine.RegisterGlobalProperty("obj", &obj, true), 0);

    // Global property is a const handle.
    std::vector<std::string> messages;
    ASSERT_GE(
        engine.Ptr()->SetMessageCallback(asFUNCTION(messageCallback), &messages, AS_NAMESPACE_QUALIFIER asCALL_CDECL), 0
    );
    ASSERT_LT(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "@obj = @obj;"), 0);
    ASSERT_THAT(messages, ElementsAre("Reference is read-only"));
    EXPECT_GE(engine.Ptr()->ClearMessageCallback(), 0);

    // Retrieve global property's property within a script.
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return obj.counter;", &counterInScript, 4), 0);

    // Is global property registered correctly?
    EXPECT_EQ(counterInScript, obj->counter);

    // Cannot update global property within a script.
    ASSERT_LT(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "++obj.counter;"), 0);

    // Is global property not updated in C++?
    EXPECT_EQ(7, obj->counter);

    // Updating global property in C++ must update it in AngelScript.
    obj->Release();

    // Retrieve global property's property within a script via const operation.
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return obj.Release();", &counterInScript, 4), 0);

    // Are changes in C++ reflected in AngelScript?
    EXPECT_EQ(counterInScript, 5);
}

TEST(AngelScriptEngineGlobalProperties, ArrayType) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());
    AS_NAMESPACE_QUALIFIER RegisterStdString(engine.Ptr());
    AS_NAMESPACE_QUALIFIER RegisterScriptArray(engine.Ptr(), false);

    constexpr auto arrayTypeDecl =
        std::define_static_string(as::GetTypeDecl<AS_NAMESPACE_QUALIFIER CScriptArray, as::StringArray>());
    ASSERT_EQ(std::string(arrayTypeDecl), "array<string>");
    auto arrayType = engine.Ptr()->GetTypeInfoByDecl(arrayTypeDecl);
    ASSERT_TRUE(arrayType);
    auto p = AS_NAMESPACE_QUALIFIER CScriptArray::Create(arrayType, 3);
    ASSERT_TRUE(p);
    as::OwnedObject<AS_NAMESPACE_QUALIFIER CScriptArray> array[[= as::StringArray]](p);
    ASSERT_TRUE(array.Ptr());

    // Register global property.
    ASSERT_GE(engine.RegisterGlobalProperty<^^array>("arr", array), 0);

    // Retrieve global property within a script.
    AS_NAMESPACE_QUALIFIER asUINT length = 0;
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return arr.length();", &length, 8), 0);

    // Is global property registered correctly?
    EXPECT_EQ(length, 3);

    // Update global property within a script.
    ASSERT_GE(
        AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "arr[0] = \"100\"; arr[1] = \"200\"; arr[2] = \"300\";"), 0
    );

    // Is global property updated in C++?
    EXPECT_EQ(*static_cast<std::string*>(array->At(0)), "100");
    EXPECT_EQ(*static_cast<std::string*>(array->At(1)), "200");
    EXPECT_EQ(*static_cast<std::string*>(array->At(2)), "300");

    // Can perform non-const operation on non-const global property.
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "arr.removeLast();"), 0);

    // Has global property updated in C++?
    EXPECT_EQ(array->GetSize(), 2);

    // Updating global property in C++ must update it in AngelScript.
    array->RemoveLast();

    // Retrieve global property within a script.
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return arr.length();", &length, 8), 0);

    // Are changes in C++ reflected in AngelScript?
    EXPECT_EQ(length, 1);
}

TEST(AngelScriptEngineGlobalProperties, ConstArrayType) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());
    AS_NAMESPACE_QUALIFIER RegisterStdString(engine.Ptr());
    AS_NAMESPACE_QUALIFIER RegisterScriptArray(engine.Ptr(), false);

    constexpr auto arrayTypeDecl =
        std::define_static_string(as::GetTypeDecl<AS_NAMESPACE_QUALIFIER CScriptArray, as::StringArray>());
    ASSERT_EQ(std::string(arrayTypeDecl), "array<string>");
    auto arrayType = engine.Ptr()->GetTypeInfoByDecl(arrayTypeDecl);
    ASSERT_TRUE(arrayType);
    auto p = AS_NAMESPACE_QUALIFIER CScriptArray::Create(arrayType, 3);
    ASSERT_TRUE(p);
    as::OwnedObject<AS_NAMESPACE_QUALIFIER CScriptArray> array[[= as::StringArray]](p);
    ASSERT_TRUE(array.Ptr());

    // Register global property.
    ASSERT_GE(engine.RegisterGlobalProperty<^^array>("arr", array, true), 0);

    // Retrieve global property within a script.
    AS_NAMESPACE_QUALIFIER asUINT length = 0;
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return arr.length();", &length, 8), 0);

    // Is global property registered correctly?
    EXPECT_EQ(length, 3);

    // You cannot update const global properties within a script.
    ASSERT_LT(
        AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "arr[0] = \"100\"; arr[1] = \"200\"; arr[2] = \"300\";"), 0
    );

    // Is global property not updated in C++?
    EXPECT_EQ(*static_cast<std::string*>(array->At(0)), "");
    EXPECT_EQ(*static_cast<std::string*>(array->At(1)), "");
    EXPECT_EQ(*static_cast<std::string*>(array->At(2)), "");

    // Cannot perform non-const operation on const global property.
    ASSERT_LT(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "arr.removeLast();"), 0);

    // Has global property updated in C++?
    EXPECT_EQ(array->GetSize(), 3);

    // Updating global property in C++ must update it in AngelScript.
    array->RemoveLast();

    // Retrieve global property within a script.
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return arr.length();", &length, 8), 0);

    // Are changes in C++ reflected in AngelScript?
    EXPECT_EQ(length, 2);
}

TEST(AngelScriptEngineGlobalProperties, ArrayHandleType) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());
    AS_NAMESPACE_QUALIFIER RegisterStdString(engine.Ptr());
    AS_NAMESPACE_QUALIFIER RegisterScriptArray(engine.Ptr(), false);

    constexpr auto arrayTypeDecl =
        std::define_static_string(as::GetTypeDecl<AS_NAMESPACE_QUALIFIER CScriptArray, as::StringArray>());
    ASSERT_EQ(std::string(arrayTypeDecl), "array<string>");
    auto arrayType = engine.Ptr()->GetTypeInfoByDecl(arrayTypeDecl);
    ASSERT_TRUE(arrayType);
    auto p = AS_NAMESPACE_QUALIFIER CScriptArray::Create(arrayType, 3);
    ASSERT_TRUE(p);
    as::OwnedObject<AS_NAMESPACE_QUALIFIER CScriptArray> array(p);
    ASSERT_TRUE(array.Ptr());
    AS_NAMESPACE_QUALIFIER CScriptArray* prop[[= as::StringArray]] = nullptr;

    // Register global property.
    ASSERT_GE(engine.RegisterGlobalProperty<^^prop>("arr", &prop), 0);

    // Retrieve global property within a script.
    bool isNull = false;
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return arr is null;", &isNull, 1), 0);

    // Is global property registered correctly?
    EXPECT_TRUE(isNull);

    // Updating global property in C++ must update it in AngelScript.
    prop = array.Ptr();

    // Retrieve global property within a script.
    AS_NAMESPACE_QUALIFIER asUINT length = 0;
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return arr.length();", &length, 8), 0);

    // Are changes in C++ reflected in AngelScript?
    EXPECT_EQ(length, 3);
}

TEST(AngelScriptEngineGlobalProperties, ArrayTypeConstSubtype) {
    as::Engine engine;
    ASSERT_TRUE(engine.HasEngine());
    AS_NAMESPACE_QUALIFIER RegisterStdString(engine.Ptr());
    AS_NAMESPACE_QUALIFIER RegisterScriptArray(engine.Ptr(), false);

    constexpr auto arrayTypeDecl =
        std::define_static_string(as::GetTypeDecl<AS_NAMESPACE_QUALIFIER CScriptArray, as::ConstStringArray>());
    ASSERT_EQ(std::string(arrayTypeDecl), "array<const string>");
    auto arrayType = engine.Ptr()->GetTypeInfoByDecl(arrayTypeDecl);
    ASSERT_TRUE(arrayType);
    auto p = AS_NAMESPACE_QUALIFIER CScriptArray::Create(arrayType, 3);
    ASSERT_TRUE(p);
    as::OwnedObject<AS_NAMESPACE_QUALIFIER CScriptArray> array[[= as::ConstStringArray]](p);
    ASSERT_TRUE(array.Ptr());

    // Register global property.
    ASSERT_GE(engine.RegisterGlobalProperty<^^array>("arr", array), 0);

    // Retrieve global property within a script.
    AS_NAMESPACE_QUALIFIER asUINT length = 0;
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return arr.length();", &length, 8), 0);

    // Is global property registered correctly?
    EXPECT_EQ(length, 3);

    // Cannot update const strings within non-const global property.
    ASSERT_LT(
        AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "arr[0] = \"100\"; arr[1] = \"200\"; arr[2] = \"300\";"), 0
    );

    // Is global property updated in C++?
    EXPECT_EQ(*static_cast<std::string*>(array->At(0)), "");
    EXPECT_EQ(*static_cast<std::string*>(array->At(1)), "");
    EXPECT_EQ(*static_cast<std::string*>(array->At(2)), "");

    // Can perform non-const operation on non-const global property.
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "arr.removeLast();"), 0);

    // Has global property updated in C++?
    EXPECT_EQ(array->GetSize(), 2);

    // Updating global property in C++ must update it in AngelScript.
    array->RemoveLast();

    // Retrieve global property within a script.
    ASSERT_GE(AS_NAMESPACE_QUALIFIER ExecuteString(engine.Ptr(), "return arr.length();", &length, 8), 0);

    // Are changes in C++ reflected in AngelScript?
    EXPECT_EQ(length, 1);
}
