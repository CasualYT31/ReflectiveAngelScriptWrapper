#include <AngelScriptWrapper/TypeDecl.hpp>
#include <AngelScriptWrapperTests/ScriptArray.hpp>
#include <AngelScriptWrapperTests/ScriptTestObject.hpp>
#include <AngelScriptWrapperTests/StaticTesting.hpp>

STATIC_ASSERT_EQ(as::TypeName<void>, "void");
STATIC_ASSERT_EQ(as::TypeName<bool>, "bool");
STATIC_ASSERT_EQ(as::TypeName<std::int8_t>, "int8");
STATIC_ASSERT_EQ(as::TypeName<std::uint8_t>, "uint8");
STATIC_ASSERT_EQ(as::TypeName<std::int16_t>, "int16");
STATIC_ASSERT_EQ(as::TypeName<std::uint16_t>, "uint16");
STATIC_ASSERT_EQ(as::TypeName<std::int32_t>, "int32");
STATIC_ASSERT_EQ(as::TypeName<std::uint32_t>, "uint32");
STATIC_ASSERT_EQ(as::TypeName<std::int64_t>, "int64");
STATIC_ASSERT_EQ(as::TypeName<std::uint64_t>, "uint64");
STATIC_ASSERT_EQ(as::TypeName<float>, "float");
STATIC_ASSERT_EQ(as::TypeName<double>, "double");
STATIC_ASSERT_EQ(as::TypeName<std::string>, "string");
STATIC_ASSERT_EQ(as::TypeName<as::ScriptTestObject>, "ScriptTestObject");

STATIC_ASSERT_EQ(as::TypeName<const bool>, "const bool");
STATIC_ASSERT_EQ(as::TypeName<const std::int8_t>, "const int8");
STATIC_ASSERT_EQ(as::TypeName<const std::uint8_t>, "const uint8");
STATIC_ASSERT_EQ(as::TypeName<const std::int16_t>, "const int16");
STATIC_ASSERT_EQ(as::TypeName<const std::uint16_t>, "const uint16");
STATIC_ASSERT_EQ(as::TypeName<const std::int32_t>, "const int32");
STATIC_ASSERT_EQ(as::TypeName<const std::uint32_t>, "const uint32");
STATIC_ASSERT_EQ(as::TypeName<const std::int64_t>, "const int64");
STATIC_ASSERT_EQ(as::TypeName<const std::uint64_t>, "const uint64");
STATIC_ASSERT_EQ(as::TypeName<const float>, "const float");
STATIC_ASSERT_EQ(as::TypeName<const double>, "const double");
STATIC_ASSERT_EQ(as::TypeName<const std::string>, "const string");
STATIC_ASSERT_EQ(as::TypeName<const as::ScriptTestObject>, "const ScriptTestObject");

STATIC_ASSERT_EQ(as::TypeName<as::ScriptTestObject*>, "ScriptTestObject@");
STATIC_ASSERT_EQ(as::TypeName<as::ScriptTestObject**>, "ScriptTestObject@");
STATIC_ASSERT_EQ(as::TypeName<const as::ScriptTestObject*>, "const ScriptTestObject@");
STATIC_ASSERT_EQ(as::TypeName<const as::ScriptTestObject* const>, "const ScriptTestObject@ const");

STATIC_ASSERT_EQ(as::TypeName<AS_NAMESPACE_QUALIFIER CDateTime>, "datetime");
STATIC_ASSERT_EQ(as::TypeName<AS_NAMESPACE_QUALIFIER Complex>, "complex");
STATIC_ASSERT_EQ(as::TypeName<AS_NAMESPACE_QUALIFIER CScriptAny>, "any");
STATIC_ASSERT_EQ(as::TypeName<AS_NAMESPACE_QUALIFIER CScriptDictionary>, "dictionary");
STATIC_ASSERT_EQ(as::TypeName<AS_NAMESPACE_QUALIFIER CScriptDictValue>, "dictionaryValue");
STATIC_ASSERT_EQ(as::TypeName<AS_NAMESPACE_QUALIFIER CScriptFile>, "file");
STATIC_ASSERT_EQ(as::TypeName<AS_NAMESPACE_QUALIFIER CScriptFileSystem>, "filesystem");
STATIC_ASSERT_EQ(as::TypeName<AS_NAMESPACE_QUALIFIER CScriptGrid>, "grid");
STATIC_ASSERT_EQ(as::TypeName<AS_NAMESPACE_QUALIFIER CScriptHandle>, "ref");
STATIC_ASSERT_EQ(as::TypeName<AS_NAMESPACE_QUALIFIER CScriptSocket>, "socket");
STATIC_ASSERT_EQ(as::TypeName<AS_NAMESPACE_QUALIFIER CScriptWeakRef>, "weakref");

static as::ScriptTestObject StringArray[[= as::subtype::String]];
static as::ScriptTestObject ConstStringArray[[= as::ConstStringArray]];
static as::ScriptTestObject StringHandleArray[[= as::StringHandleArray]];
static as::ScriptTestObject ConstStringHandleArray[[= as::ConstStringHandleArray]];
static as::ScriptTestObject ConstStringConstHandleArray[[= as::ConstStringConstHandleArray]];
static const as::ScriptTestObject StringConstArray[[= as::subtype::String]];
static const as::ScriptTestObject ConstStringConstArray[[= as::ConstStringArray]];
static const as::ScriptTestObject StringHandleConstArray[[= as::StringHandleArray]];
static const as::ScriptTestObject ConstStringHandleConstArray[[= as::ConstStringHandleArray]];
static const as::ScriptTestObject ConstStringConstHandleConstArray[[= as::ConstStringConstHandleArray]];
static as::ScriptTestObject* StringArrayHandle[[= as::subtype::String]] = nullptr;
static as::ScriptTestObject* ConstStringArrayHandle[[= as::ConstStringArray]] = nullptr;
static as::ScriptTestObject* StringHandleArrayHandle[[= as::StringHandleArray]] = nullptr;
static as::ScriptTestObject* ConstStringHandleArrayHandle[[= as::ConstStringHandleArray]] = nullptr;
static as::ScriptTestObject* ConstStringConstHandleArrayHandle[[= as::ConstStringConstHandleArray]] = nullptr;
static const as::ScriptTestObject* const StringConstArrayConstHandle[[= as::subtype::String]] = nullptr;
static const as::ScriptTestObject* const ConstStringConstArrayConstHandle[[= as::ConstStringArray]] = nullptr;
static const as::ScriptTestObject* const StringHandleConstArrayConstHandle[[= as::StringHandleArray]] = nullptr;
static const as::ScriptTestObject* const ConstStringHandleConstArrayConstHandle[[= as::ConstStringHandleArray]] =
    nullptr;
static const as::ScriptTestObject* const
    ConstStringConstHandleConstArrayConstHandle[[= as::ConstStringConstHandleArray]] = nullptr;

// clang-format off
STATIC_ASSERT_EQ(as::TypeOf<^^StringArray>, "ScriptTestObject<string>");
STATIC_ASSERT_EQ(as::TypeOf<^^ConstStringArray>, "ScriptTestObject<const string>");
STATIC_ASSERT_EQ(as::TypeOf<^^StringHandleArray>, "ScriptTestObject<string&>");
STATIC_ASSERT_EQ(as::TypeOf<^^ConstStringHandleArray>, "ScriptTestObject<const string&>");
STATIC_ASSERT_EQ(as::TypeOf<^^ConstStringConstHandleArray>, "ScriptTestObject<const string&>");
STATIC_ASSERT_EQ(as::TypeOf<^^StringConstArray>, "const ScriptTestObject<string>");
STATIC_ASSERT_EQ(as::TypeOf<^^ConstStringConstArray>, "const ScriptTestObject<const string>");
STATIC_ASSERT_EQ(as::TypeOf<^^StringHandleConstArray>, "const ScriptTestObject<string&>");
STATIC_ASSERT_EQ(as::TypeOf<^^ConstStringHandleConstArray>, "const ScriptTestObject<const string&>");
STATIC_ASSERT_EQ(as::TypeOf<^^ConstStringConstHandleConstArray>, "const ScriptTestObject<const string&>");
STATIC_ASSERT_EQ(as::TypeOf<^^StringArrayHandle>, "ScriptTestObject<string>@");
STATIC_ASSERT_EQ(as::TypeOf<^^ConstStringArrayHandle>, "ScriptTestObject<const string>@");
STATIC_ASSERT_EQ(as::TypeOf<^^StringHandleArrayHandle>, "ScriptTestObject<string&>@");
STATIC_ASSERT_EQ(as::TypeOf<^^ConstStringHandleArrayHandle>, "ScriptTestObject<const string&>@");
STATIC_ASSERT_EQ(as::TypeOf<^^ConstStringConstHandleArrayHandle>, "ScriptTestObject<const string&>@");
STATIC_ASSERT_EQ(as::TypeOf<^^StringConstArrayConstHandle>, "const ScriptTestObject<string>@ const");
STATIC_ASSERT_EQ(as::TypeOf<^^ConstStringConstArrayConstHandle>, "const ScriptTestObject<const string>@ const");
STATIC_ASSERT_EQ(as::TypeOf<^^StringHandleConstArrayConstHandle>, "const ScriptTestObject<string&>@ const");
STATIC_ASSERT_EQ(as::TypeOf<^^ConstStringHandleConstArrayConstHandle>, "const ScriptTestObject<const string&>@ const");
STATIC_ASSERT_EQ(as::TypeOf<^^ConstStringConstHandleConstArrayConstHandle>, "const ScriptTestObject<const string&>@ const");
// clang-format on

struct A {};

template <> inline constexpr std::string_view as::TypeName<A> = "AClass";

struct[[= as::Name("BRenamed")]] B {};

template <> inline constexpr std::string_view as::TypeName<B> = "BClass";

struct[[= as::Name("CNamed")]] C {};

STATIC_ASSERT_EQ(as::GetIdentifierOf<^^A>(), "A");
STATIC_ASSERT_EQ(as::GetIdentifierOf<^^B>(), "BRenamed");
STATIC_ASSERT_EQ(as::GetIdentifierOf<^^C>(), "CNamed");

STATIC_ASSERT_EQ(as::TypeName<A>, "AClass");
STATIC_ASSERT_EQ(as::TypeName<B>, "BClass");
STATIC_ASSERT_EQ(as::TypeName<C>, "CNamed");

int myFuncDef();

static AS_NAMESPACE_QUALIFIER asIScriptFunction* funcdefHandle[[= as::Funcdef(^^myFuncDef)]];
static AS_NAMESPACE_QUALIFIER asIScriptFunction* const constFuncdefHandle[[= as::Funcdef(^^myFuncDef)]] = nullptr;

STATIC_ASSERT_EQ(as::TypeOf<^^funcdefHandle>, "myFuncDef@");
STATIC_ASSERT_EQ(as::TypeOf<^^constFuncdefHandle>, "myFuncDef@ const");
// clang-format off
STATIC_ASSERT_EQ(as::detail::OverrideTypeOf<asIScriptFunction* COMMA ^^funcdefHandle>, "myFuncDef@");
STATIC_ASSERT_EQ(as::detail::OverrideTypeOf<asIScriptFunction* const COMMA ^^constFuncdefHandle>, "myFuncDef@ const");
// clang-format on

struct MyInt {
    virtual int myTest() = 0;

protected:
    void myProtectedTest() {}

private:
    void myPrivateTest() {}
};

struct MyInt2 : public MyInt {
    virtual int myTest2() = 0;

    // This myTest should be found first, except for the MyDerived case.
    // MyInt::myTest is "hidden."
    virtual int myTest() = 0;

    struct Inner {};

protected:
    void myProtectedTest2() {}

private:
    void myPrivateTest2() {}
};

struct MyInt3 : public MyInt2 {
    virtual int myTest3() = 0;

    // Padding bit fields must be ignored.
    int : 4;

    int f;

protected:
    void myProtectedTest3() {}

private:
    void myPrivateTest3() {}
};

struct Bad {
    // With the hierarchy of MyDerived, this f will be hidden by MyInt3::f,
    // except when accessed from Bad directly.
    std::string f;
};

// clang-format off
// Class hierarchies shouldn't be built like this, but I still want my function to be resilient to it.
// Ignore the warning about duplicate/inaccessible base class here.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winaccessible-base"

struct MyDerived : public MyInt, public MyInt3, public Bad {

#pragma GCC diagnostic pop // clang-format on

    virtual int myOtherTest(int) = 0;

    virtual int myOtherTest() = 0;

    MyDerived(int) {}

    MyDerived& operator+(MyDerived const&) {
        return *this;
    }

    void operator()() {}

    friend MyDerived& operator-(MyDerived&, MyDerived const&);

protected:
    void myOtherProtectedTest() {}

private:
    void myOtherPrivateTest() {}
};

MyDerived& operator-(MyDerived& a, MyDerived const& b) {
    return a;
}

using namespace std::literals;

static constexpr auto myDerivedResults = as::GetClassHierarchy<^^MyDerived>();

static_assert(myDerivedResults.size() == 1);

static_assert(myDerivedResults[0].type == ^^MyDerived);
static_assert(myDerivedResults[0].bases.size() == 4);
static_assert(myDerivedResults[0].bases[0] == ^^MyInt);
static_assert(myDerivedResults[0].bases[1] == ^^MyInt2);
static_assert(myDerivedResults[0].bases[2] == ^^MyInt3);
static_assert(myDerivedResults[0].bases[3] == ^^Bad);
static_assert(myDerivedResults[0].members.size() == 8);
// clang-format off
STATIC_ASSERT_EQ(std::meta::display_string_of(myDerivedResults[0].members[0]), "virtual int MyDerived::myOtherTest(int)"sv);
STATIC_ASSERT_EQ(std::meta::display_string_of(myDerivedResults[0].members[1]), "virtual int MyDerived::myOtherTest()"sv);
STATIC_ASSERT_EQ(std::meta::display_string_of(myDerivedResults[0].members[2]), "MyDerived& MyDerived::operator+(const MyDerived&)"sv);
STATIC_ASSERT_EQ(std::meta::display_string_of(myDerivedResults[0].members[3]), "void MyDerived::operator()()"sv);
STATIC_ASSERT_EQ(std::meta::display_string_of(myDerivedResults[0].members[4]), "virtual int MyInt::myTest()"sv);
STATIC_ASSERT_EQ(std::meta::display_string_of(myDerivedResults[0].members[5]), "virtual int MyInt3::myTest3()"sv);
STATIC_ASSERT_EQ(std::meta::display_string_of(myDerivedResults[0].members[6]), "MyInt3::f"sv);
STATIC_ASSERT_EQ(std::meta::display_string_of(myDerivedResults[0].members[7]), "virtual int MyInt2::myTest2()"sv);
// clang-format on

static constexpr auto myDerivedRecursiveResults = as::GetClassHierarchy<^^MyDerived>(true);

static_assert(myDerivedRecursiveResults.size() == 5);

static_assert(myDerivedRecursiveResults[0].type == ^^MyDerived);
static_assert(myDerivedRecursiveResults[0].bases.size() == 4);
static_assert(myDerivedRecursiveResults[0].bases[0] == ^^MyInt);
static_assert(myDerivedRecursiveResults[0].bases[1] == ^^MyInt2);
static_assert(myDerivedRecursiveResults[0].bases[2] == ^^MyInt3);
static_assert(myDerivedRecursiveResults[0].bases[3] == ^^Bad);
static_assert(myDerivedRecursiveResults[0].members.size() == 8);
// clang-format off
STATIC_ASSERT_EQ(std::meta::display_string_of(myDerivedRecursiveResults[0].members[0]), "virtual int MyDerived::myOtherTest(int)"sv);
STATIC_ASSERT_EQ(std::meta::display_string_of(myDerivedRecursiveResults[0].members[1]), "virtual int MyDerived::myOtherTest()"sv);
STATIC_ASSERT_EQ(std::meta::display_string_of(myDerivedRecursiveResults[0].members[2]), "MyDerived& MyDerived::operator+(const MyDerived&)"sv);
STATIC_ASSERT_EQ(std::meta::display_string_of(myDerivedRecursiveResults[0].members[3]), "void MyDerived::operator()()"sv);
STATIC_ASSERT_EQ(std::meta::display_string_of(myDerivedRecursiveResults[0].members[4]), "virtual int MyInt::myTest()"sv);
STATIC_ASSERT_EQ(std::meta::display_string_of(myDerivedRecursiveResults[0].members[5]), "virtual int MyInt3::myTest3()"sv);
STATIC_ASSERT_EQ(std::meta::display_string_of(myDerivedRecursiveResults[0].members[6]), "MyInt3::f"sv);
STATIC_ASSERT_EQ(std::meta::display_string_of(myDerivedRecursiveResults[0].members[7]), "virtual int MyInt2::myTest2()"sv);
// clang-format on

static_assert(myDerivedRecursiveResults[1].type == ^^MyInt);
static_assert(myDerivedRecursiveResults[1].bases.size() == 0);
static_assert(myDerivedRecursiveResults[1].members.size() == 1);
// clang-format off
STATIC_ASSERT_EQ(std::meta::display_string_of(myDerivedRecursiveResults[1].members[0]), "virtual int MyInt::myTest()"sv);
// clang-format on

static_assert(myDerivedRecursiveResults[2].type == ^^MyInt3);
static_assert(myDerivedRecursiveResults[2].bases.size() == 2);
static_assert(myDerivedRecursiveResults[2].bases[0] == ^^MyInt);
static_assert(myDerivedRecursiveResults[2].bases[1] == ^^MyInt2);
static_assert(myDerivedRecursiveResults[2].members.size() == 4);
// clang-format off
STATIC_ASSERT_EQ(std::meta::display_string_of(myDerivedRecursiveResults[2].members[0]), "virtual int MyInt3::myTest3()"sv);
STATIC_ASSERT_EQ(std::meta::display_string_of(myDerivedRecursiveResults[2].members[1]), "MyInt3::f"sv);
STATIC_ASSERT_EQ(std::meta::display_string_of(myDerivedRecursiveResults[2].members[2]), "virtual int MyInt2::myTest2()"sv);
STATIC_ASSERT_EQ(std::meta::display_string_of(myDerivedRecursiveResults[2].members[3]), "virtual int MyInt2::myTest()"sv);
// clang-format on

static_assert(myDerivedRecursiveResults[3].type == ^^MyInt2);
static_assert(myDerivedRecursiveResults[3].bases.size() == 1);
static_assert(myDerivedRecursiveResults[3].bases[0] == ^^MyInt);
static_assert(myDerivedRecursiveResults[3].members.size() == 2);
// clang-format off
STATIC_ASSERT_EQ(std::meta::display_string_of(myDerivedRecursiveResults[3].members[0]), "virtual int MyInt2::myTest2()"sv);
STATIC_ASSERT_EQ(std::meta::display_string_of(myDerivedRecursiveResults[3].members[1]), "virtual int MyInt2::myTest()"sv);
// clang-format on

static_assert(myDerivedRecursiveResults[4].type == ^^Bad);
static_assert(myDerivedRecursiveResults[4].bases.size() == 0);
static_assert(myDerivedRecursiveResults[4].members.size() == 1);
// clang-format off
STATIC_ASSERT_EQ(std::meta::display_string_of(myDerivedRecursiveResults[4].members[0]), "Bad::f"sv);
// clang-format on

static AS_NAMESPACE_QUALIFIER asIScriptObject* objectHandle[[= as::Interface(^^MyDerived)]];
static AS_NAMESPACE_QUALIFIER asIScriptObject* const constObjectHandle[[= as::Interface(^^MyDerived)]] = nullptr;

STATIC_ASSERT_EQ(as::TypeOf<^^objectHandle>, "MyDerived@");
STATIC_ASSERT_EQ(as::TypeOf<^^constObjectHandle>, "MyDerived@ const");
// clang-format off
STATIC_ASSERT_EQ(as::detail::OverrideTypeOf<asIScriptObject* COMMA ^^objectHandle>, "MyDerived@");
STATIC_ASSERT_EQ(as::detail::OverrideTypeOf<asIScriptObject* const COMMA ^^constObjectHandle>, "MyDerived@ const");
// clang-format on
