#include <AngelScriptWrapper/TypeDecl.hpp>
#include <AngelScriptWrapperTests/ScriptArray.hpp>
#include <AngelScriptWrapperTests/ScriptDebugging.hpp>
#include <AngelScriptWrapperTests/ScriptTestObject.hpp>

static_assert(as::SubType().subTypes.size() == 0);
static_assert(as::SubType<int>().subTypes[0] == ^^int);
static_assert(as::SubTypes<int, AS_NAMESPACE_QUALIFIER CScriptArray*, std::string>().subTypes[1] == ^^AS_NAMESPACE_QUALIFIER CScriptArray*);
// <array<string>, array<array<string>>>
#define NESTED                                                                                                              \
    as::SubTypes<                                                                                                           \
        as::Tmpl<AS_NAMESPACE_QUALIFIER CScriptArray*, std::string>,                                                        \
        as::Tmpl<AS_NAMESPACE_QUALIFIER CScriptArray*, as::Tmpl<AS_NAMESPACE_QUALIFIER CScriptArray*, std::string>>>()
static_assert(NESTED.subTypes.size() == 2);
static_assert(NESTED.subTypes.size() == NESTED.recursiveSub.size());
static_assert(NESTED.subTypes[0] == ^^AS_NAMESPACE_QUALIFIER CScriptArray*);
static_assert(NESTED.recursiveSub[0].subTypes[0] == std::meta::dealias(^^std::string));
static_assert(NESTED.subTypes[1] == ^^AS_NAMESPACE_QUALIFIER CScriptArray*);
static_assert(NESTED.recursiveSub[1].subTypes[0] == ^^AS_NAMESPACE_QUALIFIER CScriptArray*);
static_assert(NESTED.recursiveSub[1].recursiveSub[0].subTypes[0] == std::meta::dealias(^^std::string));
#undef NESTED

static_assert(!as::IsRefType<int>());
static_assert(!as::IsRefType<std::string>());
static_assert(as::IsRefType<AS_NAMESPACE_QUALIFIER CScriptArray>());
static_assert(as::IsRefType<as::ScriptTestObject>());

static_assert(!as::IsRefType<int*>());
static_assert(!as::IsRefType<std::string*>());
static_assert(as::IsRefType<AS_NAMESPACE_QUALIFIER CScriptArray*>());
static_assert(as::IsRefType<as::ScriptTestObject*>());

static_assert(!as::IsRefType<int&>());
static_assert(!as::IsRefType<std::string&>());
static_assert(as::IsRefType<AS_NAMESPACE_QUALIFIER CScriptArray&>());
static_assert(as::IsRefType<as::ScriptTestObject&>());

static_assert(!as::IsRefType<const int&>());
static_assert(!as::IsRefType<const std::string&>());
static_assert(as::IsRefType<const AS_NAMESPACE_QUALIFIER CScriptArray&>());
static_assert(as::IsRefType<const as::ScriptTestObject&>());

static_assert(!as::IsRefType<const int* const*>());
static_assert(!as::IsRefType<const std::string* const*>());
static_assert(as::IsRefType<const AS_NAMESPACE_QUALIFIER CScriptArray* const*>());
static_assert(as::IsRefType<const as::ScriptTestObject* const*>());

void cdecl();

struct HasStaticMethod {
    static inline void myStatic() {}
};

struct HasNonStaticMethod {
    inline void myNonStatic() {}
};

[[= as::CallConv(AS_NAMESPACE_QUALIFIER asCALL_STDCALL)]] void customCallConv() {}

struct HasCustomCallConv {
    [[= as::CallConv(AS_NAMESPACE_QUALIFIER asCALL_GENERIC)]] inline void custom() {}
};

// You cannot have more than one CallConv annotation:
// namespace test {
// [[ = as::CallConv(0), = as::CallConv(1) ]] void m();
// }
// static_assert(as::GetFuncCallConv<^^test::m>() == 1);

static_assert(as::GetFuncCallConv<^^cdecl>() == AS_NAMESPACE_QUALIFIER asCALL_CDECL);
static_assert(as::GetFuncCallConv<^^HasStaticMethod::myStatic>() == AS_NAMESPACE_QUALIFIER asCALL_CDECL);
static_assert(as::GetFuncCallConv<^^HasNonStaticMethod::myNonStatic>() == AS_NAMESPACE_QUALIFIER asCALL_THISCALL);
static_assert(as::GetFuncCallConv<^^customCallConv>() == AS_NAMESPACE_QUALIFIER asCALL_STDCALL);
static_assert(as::GetFuncCallConv<^^HasCustomCallConv::custom>() == AS_NAMESPACE_QUALIFIER asCALL_GENERIC);

static_assert(as::GetTypeDecl<void>() == "void");
static_assert(as::GetTypeDecl<bool>() == "bool");
static_assert(as::GetTypeDecl<std::int8_t>() == "int8");
static_assert(as::GetTypeDecl<std::uint8_t>() == "uint8");
static_assert(as::GetTypeDecl<std::int16_t>() == "int16");
static_assert(as::GetTypeDecl<std::uint16_t>() == "uint16");
static_assert(as::GetTypeDecl<std::int32_t>() == "int32");
static_assert(as::GetTypeDecl<std::uint32_t>() == "uint32");
static_assert(as::GetTypeDecl<std::int64_t>() == "int64");
static_assert(as::GetTypeDecl<std::uint64_t>() == "uint64");
static_assert(as::GetTypeDecl<float>() == "float");
static_assert(as::GetTypeDecl<double>() == "double");
static_assert(as::GetTypeDecl<std::string>() == "string");
static_assert(as::GetTypeDecl<as::ScriptTestObject>() == "ScriptTestObject");

static_assert(as::GetTypeDecl<const bool>() == "const bool");
static_assert(as::GetTypeDecl<const std::int8_t>() == "const int8");
static_assert(as::GetTypeDecl<const std::uint8_t>() == "const uint8");
static_assert(as::GetTypeDecl<const std::int16_t>() == "const int16");
static_assert(as::GetTypeDecl<const std::uint16_t>() == "const uint16");
static_assert(as::GetTypeDecl<const std::int32_t>() == "const int32");
static_assert(as::GetTypeDecl<const std::uint32_t>() == "const uint32");
static_assert(as::GetTypeDecl<const std::int64_t>() == "const int64");
static_assert(as::GetTypeDecl<const std::uint64_t>() == "const uint64");
static_assert(as::GetTypeDecl<const float>() == "const float");
static_assert(as::GetTypeDecl<const double>() == "const double");
static_assert(as::GetTypeDecl<const std::string>() == "const string");
static_assert(as::GetTypeDecl<const as::ScriptTestObject>() == "const ScriptTestObject");

static_assert(as::GetTypeDecl<as::ScriptTestObject*>() == "ScriptTestObject@", as::GetTypeDecl<as::ScriptTestObject*>());
static_assert(as::GetTypeDecl<as::ScriptTestObject**>() == "ScriptTestObject@@");
static_assert(as::GetTypeDecl<const as::ScriptTestObject*>() == "const ScriptTestObject@");
static_assert(as::GetTypeDecl<const as::ScriptTestObject* const>() == "const ScriptTestObject@ const");

static as::ScriptTestObject StringArray[[= as::StringArray]];
static as::ScriptTestObject ConstStringArray[[= as::ConstStringArray]];
static as::ScriptTestObject StringHandleArray[[= as::StringHandleArray]];
static as::ScriptTestObject ConstStringHandleArray[[= as::ConstStringHandleArray]];
static as::ScriptTestObject ConstStringConstHandleArray[[= as::ConstStringConstHandleArray]];
static const as::ScriptTestObject StringConstArray[[= as::StringArray]];
static const as::ScriptTestObject ConstStringConstArray[[= as::ConstStringArray]];
static const as::ScriptTestObject StringHandleConstArray[[= as::StringHandleArray]];
static const as::ScriptTestObject ConstStringHandleConstArray[[= as::ConstStringHandleArray]];
static const as::ScriptTestObject ConstStringConstHandleConstArray[[= as::ConstStringConstHandleArray]];
static as::ScriptTestObject* StringArrayHandle[[= as::StringArray]] = nullptr;
static as::ScriptTestObject* ConstStringArrayHandle[[= as::ConstStringArray]] = nullptr;
static as::ScriptTestObject* StringHandleArrayHandle[[= as::StringHandleArray]] = nullptr;
static as::ScriptTestObject* ConstStringHandleArrayHandle[[= as::ConstStringHandleArray]] = nullptr;
static as::ScriptTestObject* ConstStringConstHandleArrayHandle[[= as::ConstStringConstHandleArray]] = nullptr;
static const as::ScriptTestObject* const StringConstArrayConstHandle[[= as::StringArray]] = nullptr;
static const as::ScriptTestObject* const ConstStringConstArrayConstHandle[[= as::ConstStringArray]] = nullptr;
static const as::ScriptTestObject* const StringHandleConstArrayConstHandle[[= as::StringHandleArray]] = nullptr;
static const as::ScriptTestObject* const ConstStringHandleConstArrayConstHandle[[= as::ConstStringHandleArray]] = nullptr;
static const as::ScriptTestObject* const ConstStringConstHandleConstArrayConstHandle[[= as::ConstStringConstHandleArray]] =
    nullptr;

// clang-format off
STATIC_ASSERT_EQ(as::GetTypeDecl<as::ScriptTestObject COMMA as::TmplSubTypes{}>(), "ScriptTestObject");
STATIC_ASSERT_EQ(as::GetTypeDecl<^^StringArray>(), "ScriptTestObject<string>");
STATIC_ASSERT_EQ(as::GetTypeDecl<^^ConstStringArray>(), "ScriptTestObject<const string>");
STATIC_ASSERT_EQ(as::GetTypeDecl<^^StringHandleArray>(), "ScriptTestObject<string&>");
STATIC_ASSERT_EQ(as::GetTypeDecl<^^ConstStringHandleArray>(), "ScriptTestObject<const string&>");
STATIC_ASSERT_EQ(as::GetTypeDecl<^^ConstStringConstHandleArray>(), "ScriptTestObject<const string&>");
STATIC_ASSERT_EQ(as::GetTypeDecl<^^StringConstArray>(), "const ScriptTestObject<string>");
STATIC_ASSERT_EQ(as::GetTypeDecl<^^ConstStringConstArray>(), "const ScriptTestObject<const string>");
STATIC_ASSERT_EQ(as::GetTypeDecl<^^StringHandleConstArray>(), "const ScriptTestObject<string&>");
STATIC_ASSERT_EQ(as::GetTypeDecl<^^ConstStringHandleConstArray>(), "const ScriptTestObject<const string&>");
STATIC_ASSERT_EQ(as::GetTypeDecl<^^ConstStringConstHandleConstArray>(), "const ScriptTestObject<const string&>");
STATIC_ASSERT_EQ(as::GetTypeDecl<^^StringArrayHandle>(), "ScriptTestObject<string>@");
STATIC_ASSERT_EQ(as::GetTypeDecl<^^ConstStringArrayHandle>(), "ScriptTestObject<const string>@");
STATIC_ASSERT_EQ(as::GetTypeDecl<^^StringHandleArrayHandle>(), "ScriptTestObject<string&>@");
STATIC_ASSERT_EQ(as::GetTypeDecl<^^ConstStringHandleArrayHandle>(), "ScriptTestObject<const string&>@");
STATIC_ASSERT_EQ(as::GetTypeDecl<^^ConstStringConstHandleArrayHandle>(), "ScriptTestObject<const string&>@");
STATIC_ASSERT_EQ(as::GetTypeDecl<^^StringConstArrayConstHandle>(), "const ScriptTestObject<string>@ const");
STATIC_ASSERT_EQ(as::GetTypeDecl<^^ConstStringConstArrayConstHandle>(), "const ScriptTestObject<const string>@ const");
STATIC_ASSERT_EQ(as::GetTypeDecl<^^StringHandleConstArrayConstHandle>(), "const ScriptTestObject<string&>@ const");
STATIC_ASSERT_EQ(as::GetTypeDecl<^^ConstStringHandleConstArrayConstHandle>(), "const ScriptTestObject<const string&>@ const");
STATIC_ASSERT_EQ(as::GetTypeDecl<^^ConstStringConstHandleConstArrayConstHandle>(), "const ScriptTestObject<const string&>@ const");
// clang-format on

STATIC_ASSERT_EQ(as::GetTypeDecl<AS_NAMESPACE_QUALIFIER CDateTime>(), "datetime");
STATIC_ASSERT_EQ(as::GetTypeDecl<AS_NAMESPACE_QUALIFIER Complex>(), "complex");
STATIC_ASSERT_EQ(as::GetTypeDecl<AS_NAMESPACE_QUALIFIER CScriptAny>(), "any");
STATIC_ASSERT_EQ(as::GetTypeDecl<AS_NAMESPACE_QUALIFIER CScriptDictionary>(), "dictionary");
STATIC_ASSERT_EQ(as::GetTypeDecl<AS_NAMESPACE_QUALIFIER CScriptDictValue>(), "dictionaryValue");
STATIC_ASSERT_EQ(as::GetTypeDecl<AS_NAMESPACE_QUALIFIER CScriptFile>(), "file");
STATIC_ASSERT_EQ(as::GetTypeDecl<AS_NAMESPACE_QUALIFIER CScriptFileSystem>(), "filesystem");
STATIC_ASSERT_EQ(as::GetTypeDecl<AS_NAMESPACE_QUALIFIER CScriptGrid>(), "grid");
STATIC_ASSERT_EQ(as::GetTypeDecl<AS_NAMESPACE_QUALIFIER CScriptHandle>(), "ref");
STATIC_ASSERT_EQ(as::GetTypeDecl<AS_NAMESPACE_QUALIFIER CScriptSocket>(), "socket");
STATIC_ASSERT_EQ(as::GetTypeDecl<AS_NAMESPACE_QUALIFIER CScriptWeakRef>(), "weakref");
