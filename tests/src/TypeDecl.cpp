#include <AngelScriptWrapper/TypeDecl.hpp>
#include <AngelScriptWrapperTests/ScriptArray.hpp>
#include <AngelScriptWrapperTests/ScriptTestObject.hpp>

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

// Technically, strings are value types by default in AngelScript,
// but we just want to test @ handling here.
using StringHandleArray = as::Specialize<AS_NAMESPACE_QUALIFIER CScriptArray, std::string*>;
using ConstStringHandleArray = as::Specialize<AS_NAMESPACE_QUALIFIER CScriptArray, const std::string*>;
using ConstStringConstHandleArray = as::Specialize<AS_NAMESPACE_QUALIFIER CScriptArray, const std::string* const>;
using ConstStringConstArray = as::Specialize<const AS_NAMESPACE_QUALIFIER CScriptArray, const std::string>;
using StringHandleConstArray = as::Specialize<const AS_NAMESPACE_QUALIFIER CScriptArray, std::string*>;
using ConstStringHandleConstArray = as::Specialize<const AS_NAMESPACE_QUALIFIER CScriptArray, const std::string*>;
using ConstStringConstHandleConstArray = as::Specialize<const AS_NAMESPACE_QUALIFIER CScriptArray, const std::string* const>;
using ConstStringArrayHandle = as::Specialize<AS_NAMESPACE_QUALIFIER CScriptArray*, const std::string>;
using StringHandleArrayHandle = as::Specialize<AS_NAMESPACE_QUALIFIER CScriptArray*, std::string*>;
using ConstStringHandleArrayHandle = as::Specialize<AS_NAMESPACE_QUALIFIER CScriptArray*, const std::string*>;
using ConstStringConstHandleArrayHandle = as::Specialize<AS_NAMESPACE_QUALIFIER CScriptArray*, const std::string* const>;
using StringConstArrayConstHandle = as::Specialize<const AS_NAMESPACE_QUALIFIER CScriptArray* const, std::string>;
using ConstStringConstArrayConstHandle = as::Specialize<const AS_NAMESPACE_QUALIFIER CScriptArray* const, const std::string>;
using StringHandleConstArrayConstHandle = as::Specialize<const AS_NAMESPACE_QUALIFIER CScriptArray* const, std::string*>;
using ConstStringHandleConstArrayConstHandle =
    as::Specialize<const AS_NAMESPACE_QUALIFIER CScriptArray* const, const std::string*>;
using ConstStringConstHandleConstArrayConstHandle =
    as::Specialize<const AS_NAMESPACE_QUALIFIER CScriptArray* const, const std::string* const>;
using StringArrayHandleHandle = as::Specialize<AS_NAMESPACE_QUALIFIER CScriptArray**, std::string>;

static_assert(as::GetTypeDecl<as::StringArray::T>() == "array");
static_assert(as::GetTypeDecl<^^as::StringArray::T>() == "array<string>");
static_assert(as::GetTypeDecl<^^as::ConstStringArray::T>() == "array<const string>");
static_assert(as::GetTypeDecl<^^StringHandleArray::T>() == "array<string&>");
static_assert(as::GetTypeDecl<^^ConstStringHandleArray::T>() == "array<const string&>");
static_assert(as::GetTypeDecl<^^ConstStringConstHandleArray::T>() == "array<const string&>");
static_assert(as::GetTypeDecl<^^as::StringConstArray::T>() == "const array<string>");
static_assert(as::GetTypeDecl<^^ConstStringConstArray::T>() == "const array<const string>");
static_assert(as::GetTypeDecl<^^StringHandleConstArray::T>() == "const array<string&>");
static_assert(as::GetTypeDecl<^^ConstStringHandleConstArray::T>() == "const array<const string&>");
static_assert(as::GetTypeDecl<^^ConstStringConstHandleConstArray::T>() == "const array<const string&>");
static_assert(as::GetTypeDecl<^^as::StringArrayHandle::T>() == "array<string>@");
static_assert(as::GetTypeDecl<^^ConstStringArrayHandle::T>() == "array<const string>@");
static_assert(as::GetTypeDecl<^^StringHandleArrayHandle::T>() == "array<string&>@");
static_assert(as::GetTypeDecl<^^ConstStringHandleArrayHandle::T>() == "array<const string&>@");
static_assert(as::GetTypeDecl<^^ConstStringConstHandleArrayHandle::T>() == "array<const string&>@");
static_assert(as::GetTypeDecl<^^StringConstArrayConstHandle::T>() == "const array<string>@ const");
static_assert(as::GetTypeDecl<^^ConstStringConstArrayConstHandle::T>() == "const array<const string>@ const");
static_assert(as::GetTypeDecl<^^StringHandleConstArrayConstHandle::T>() == "const array<string&>@ const");
static_assert(as::GetTypeDecl<^^ConstStringHandleConstArrayConstHandle::T>() == "const array<const string&>@ const");
static_assert(as::GetTypeDecl<^^ConstStringConstHandleConstArrayConstHandle::T>() == "const array<const string&>@ const");

static_assert(as::GetTypeDecl<AS_NAMESPACE_QUALIFIER CDateTime>() == "datetime");
static_assert(as::GetTypeDecl<AS_NAMESPACE_QUALIFIER Complex>() == "complex");
static_assert(as::GetTypeDecl<AS_NAMESPACE_QUALIFIER CScriptAny>() == "any");
static_assert(as::GetTypeDecl<AS_NAMESPACE_QUALIFIER CScriptDictionary>() == "dictionary");
static_assert(as::GetTypeDecl<AS_NAMESPACE_QUALIFIER CScriptDictValue>() == "dictionaryValue");
static_assert(as::GetTypeDecl<AS_NAMESPACE_QUALIFIER CScriptFile>() == "file");
static_assert(as::GetTypeDecl<AS_NAMESPACE_QUALIFIER CScriptFileSystem>() == "filesystem");
static_assert(as::GetTypeDecl<AS_NAMESPACE_QUALIFIER CScriptGrid>() == "grid");
static_assert(as::GetTypeDecl<AS_NAMESPACE_QUALIFIER CScriptHandle>() == "ref");
static_assert(as::GetTypeDecl<AS_NAMESPACE_QUALIFIER CScriptSocket>() == "socket");
static_assert(as::GetTypeDecl<AS_NAMESPACE_QUALIFIER CScriptWeakRef>() == "weakref");
