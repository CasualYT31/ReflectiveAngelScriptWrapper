/**
 * @file Annotations.cpp
 * Test annotation features.
 */

#include <AngelScriptWrapper/Annotations.hpp>
#include <AngelScriptWrapperTests/ScriptTestObject.hpp>
#include <AngelScriptWrapperTests/StaticTesting.hpp>

int a1;
const int a2 = 0;
int a3[[= as::GlobalConst]];
int a4[[= as::GlobalNotConst]];
int a5[[ = as::GlobalConst, = as::GlobalNotConst ]];

static_assert(!as::IsGlobalConstQualified<^^a1, false>());
static_assert(!as::IsGlobalConstQualified<^^a2, false>());
static_assert(as::IsGlobalConstQualified<^^a3, false>());
static_assert(!as::IsGlobalConstQualified<^^a4, false>());

static_assert(as::IsGlobalConstQualified<^^a1, true>());
static_assert(as::IsGlobalConstQualified<^^a2, true>());
static_assert(as::IsGlobalConstQualified<^^a3, true>());
static_assert(!as::IsGlobalConstQualified<^^a4, true>());

// Fails compilation due to mix of annotations:
// static_assert(!as::IsGlobalConstQualified<^^a5, false>());

inline void f[[= as::Name("func")]](int p) {}

static_assert(as::GetAnnotations<^^f COMMA as::Rename>().size() == 1);
static_assert(std::string_view(as::ExtractAnnotation<^^f COMMA as::Rename>()->to) == "func");
static_assert(as::HasAnnotation<^^f COMMA as::Rename>());
// clang-format off
static_assert(as::GetAnnotations<^^f COMMA ^^as::Rename>().size() == 1);
static_assert(std::string_view(as::ExtractAnnotation<^^f COMMA ^^as::Rename>()->to) == "func");
static_assert(as::HasAnnotation<^^f COMMA ^^as::Rename>());
// clang-format on

static_assert(as::GetAnnotations<^^f COMMA as::SubTypes>().size() == 0);
static_assert(!as::HasAnnotation<^^f COMMA as::SubTypes>());
// clang-format off
static_assert(as::GetAnnotations<^^f COMMA ^^as::SubTypes>().size() == 0);
static_assert(!as::HasAnnotation<^^f COMMA ^^as::SubTypes>());
// clang-format on

static_assert(as::AtLeastOneOf<>());
static_assert(as::AtLeastOneOf<false>());
static_assert(as::AtLeastOneOf<true>());
static_assert(as::AtLeastOneOf<false, false>());
static_assert(as::AtLeastOneOf<true, false>());
static_assert(as::AtLeastOneOf<false, true>());
static_assert(!as::AtLeastOneOf<true, true>());
static_assert(as::AtLeastOneOf<false, false, false>());
static_assert(as::AtLeastOneOf<true, false, false>());
static_assert(as::AtLeastOneOf<false, true, false>());
static_assert(as::AtLeastOneOf<false, false, true>());
static_assert(!as::AtLeastOneOf<true, false, true>());
static_assert(!as::AtLeastOneOf<false, true, true>());
static_assert(!as::AtLeastOneOf<true, true, true>());

static_assert(as::SubTypeList().subTypes.size() == 0);
static_assert(as::SubTypeList<int>().subTypes[0] == ^^int);
static_assert(as::SubTypeList<int, AS_NAMESPACE_QUALIFIER CScriptArray*, std::string>().subTypes[1] == ^^AS_NAMESPACE_QUALIFIER CScriptArray*);
// <array<string>, array<array<string>>>
#define NESTED                                                                                                         \
    as::SubTypeList<                                                                                                   \
        as::Tmpl<AS_NAMESPACE_QUALIFIER CScriptArray*, std::string>,                                                   \
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
