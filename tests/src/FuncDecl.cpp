#include <AngelScriptWrapper/FuncDecl.hpp>
#include <AngelScriptWrapperTests/ScriptArray.hpp>
#include <AngelScriptWrapperTests/StaticTesting.hpp>

STATIC_ASSERT_EQ(std::string(as::DefVal("5").value), "5");
STATIC_ASSERT_EQ(std::string(as::DefVal("\"5\"").value), "\"5\"");

namespace call_conv {
inline void generic[[= as::Generic("int add(int, int)")]](asIScriptGeneric*) {}

inline void Default() {}

inline void defaultFirst[[= as::ObjFirst]]() {}

inline void defaultLast[[= as::ObjLast]]() {}

inline void cDecl[[= as::CDecl]]() {}

inline void cDeclFirst[[ = as::CDecl, = as::ObjFirst ]]() {}

inline void cDeclLast[[ = as::CDecl, = as::ObjLast ]]() {}

inline void stdCall[[= as::StdCall]]() {}

inline void explicitGeneric[[= as::Generic()]]() {}

struct C {
    inline void nonStatic() {}

    inline void nonStaticFirst[[= as::ObjFirst]]() {}

    inline void nonStaticLast[[= as::ObjLast]]() {}

    inline void nonStaticGeneric(asIScriptGeneric*) {}

    inline static void generic[[= as::Generic("int add(int, int)")]](asIScriptGeneric*) {}

    inline static void Default() {}

    inline static void defaultFirst[[= as::ObjFirst]]() {}

    inline static void defaultLast[[= as::ObjLast]]() {}

    inline static void cDecl[[= as::CDecl]]() {}

    inline static void cDeclFirst[[ = as::CDecl, = as::ObjFirst ]]() {}

    inline static void cDeclLast[[ = as::CDecl, = as::ObjLast ]]() {}

    inline static void stdCall[[= as::StdCall]]() {}

    inline static void explicitGeneric[[= as::Generic()]]() {}
};
} // namespace call_conv

// clang-format off

static_assert(as::FuncCallConv<^^call_conv::generic, AS_NAMESPACE_QUALIFIER asCALL_CDECL>().callConv == AS_NAMESPACE_QUALIFIER asCALL_GENERIC);
static_assert(as::FuncCallConv<^^call_conv::Default, AS_NAMESPACE_QUALIFIER asCALL_CDECL>().callConv == AS_NAMESPACE_QUALIFIER asCALL_CDECL);
static_assert(as::FuncCallConv<^^call_conv::defaultFirst, AS_NAMESPACE_QUALIFIER asCALL_CDECL>().callConv == AS_NAMESPACE_QUALIFIER asCALL_CDECL_OBJFIRST);
static_assert(as::FuncCallConv<^^call_conv::defaultLast, AS_NAMESPACE_QUALIFIER asCALL_CDECL>().callConv == AS_NAMESPACE_QUALIFIER asCALL_CDECL_OBJLAST);
static_assert(as::FuncCallConv<^^call_conv::cDecl, AS_NAMESPACE_QUALIFIER asCALL_CDECL>().callConv == AS_NAMESPACE_QUALIFIER asCALL_CDECL);
static_assert(as::FuncCallConv<^^call_conv::cDeclFirst, AS_NAMESPACE_QUALIFIER asCALL_CDECL>().callConv == AS_NAMESPACE_QUALIFIER asCALL_CDECL_OBJFIRST);
static_assert(as::FuncCallConv<^^call_conv::cDeclLast, AS_NAMESPACE_QUALIFIER asCALL_CDECL>().callConv == AS_NAMESPACE_QUALIFIER asCALL_CDECL_OBJLAST);
static_assert(as::FuncCallConv<^^call_conv::stdCall, AS_NAMESPACE_QUALIFIER asCALL_CDECL>().callConv == AS_NAMESPACE_QUALIFIER asCALL_STDCALL);
static_assert(as::FuncCallConv<^^call_conv::explicitGeneric, AS_NAMESPACE_QUALIFIER asCALL_CDECL>().callConv == AS_NAMESPACE_QUALIFIER asCALL_GENERIC);
static_assert(as::FuncCallConv<^^call_conv::C::nonStatic, AS_NAMESPACE_QUALIFIER asCALL_CDECL>().callConv == AS_NAMESPACE_QUALIFIER asCALL_THISCALL);
static_assert(as::FuncCallConv<^^call_conv::C::nonStaticFirst, AS_NAMESPACE_QUALIFIER asCALL_CDECL>().callConv == AS_NAMESPACE_QUALIFIER asCALL_THISCALL_OBJFIRST);
static_assert(as::FuncCallConv<^^call_conv::C::nonStaticLast, AS_NAMESPACE_QUALIFIER asCALL_CDECL>().callConv == AS_NAMESPACE_QUALIFIER asCALL_THISCALL_OBJLAST);
static_assert(as::FuncCallConv<^^call_conv::C::generic, AS_NAMESPACE_QUALIFIER asCALL_CDECL>().callConv == AS_NAMESPACE_QUALIFIER asCALL_GENERIC);
static_assert(as::FuncCallConv<^^call_conv::C::Default, AS_NAMESPACE_QUALIFIER asCALL_CDECL>().callConv == AS_NAMESPACE_QUALIFIER asCALL_CDECL);
static_assert(as::FuncCallConv<^^call_conv::C::defaultFirst, AS_NAMESPACE_QUALIFIER asCALL_CDECL>().callConv == AS_NAMESPACE_QUALIFIER asCALL_CDECL_OBJFIRST);
static_assert(as::FuncCallConv<^^call_conv::C::defaultLast, AS_NAMESPACE_QUALIFIER asCALL_CDECL>().callConv == AS_NAMESPACE_QUALIFIER asCALL_CDECL_OBJLAST);
static_assert(as::FuncCallConv<^^call_conv::C::cDecl, AS_NAMESPACE_QUALIFIER asCALL_CDECL>().callConv == AS_NAMESPACE_QUALIFIER asCALL_CDECL);
static_assert(as::FuncCallConv<^^call_conv::C::cDeclFirst, AS_NAMESPACE_QUALIFIER asCALL_CDECL>().callConv == AS_NAMESPACE_QUALIFIER asCALL_CDECL_OBJFIRST);
static_assert(as::FuncCallConv<^^call_conv::C::cDeclLast, AS_NAMESPACE_QUALIFIER asCALL_CDECL>().callConv == AS_NAMESPACE_QUALIFIER asCALL_CDECL_OBJLAST);
static_assert(as::FuncCallConv<^^call_conv::C::stdCall, AS_NAMESPACE_QUALIFIER asCALL_CDECL>().callConv == AS_NAMESPACE_QUALIFIER asCALL_STDCALL);
static_assert(as::FuncCallConv<^^call_conv::C::explicitGeneric, AS_NAMESPACE_QUALIFIER asCALL_CDECL>().callConv == AS_NAMESPACE_QUALIFIER asCALL_GENERIC);

static_assert(as::FuncCallConv<^^call_conv::generic, AS_NAMESPACE_QUALIFIER asCALL_STDCALL>().callConv == AS_NAMESPACE_QUALIFIER asCALL_GENERIC);
static_assert(as::FuncCallConv<^^call_conv::Default, AS_NAMESPACE_QUALIFIER asCALL_STDCALL>().callConv == AS_NAMESPACE_QUALIFIER asCALL_STDCALL);
// Not currently supported by AngelScript:
// static_assert(as::FuncCallConv<^^call_conv::defaultFirst, AS_NAMESPACE_QUALIFIER asCALL_STDCALL>().callConv == AS_NAMESPACE_QUALIFIER asCALL_STDCALL);
// static_assert(as::FuncCallConv<^^call_conv::defaultLast, AS_NAMESPACE_QUALIFIER asCALL_STDCALL>().callConv == AS_NAMESPACE_QUALIFIER asCALL_STDCALL);
static_assert(as::FuncCallConv<^^call_conv::cDecl, AS_NAMESPACE_QUALIFIER asCALL_STDCALL>().callConv == AS_NAMESPACE_QUALIFIER asCALL_CDECL);
static_assert(as::FuncCallConv<^^call_conv::cDeclFirst, AS_NAMESPACE_QUALIFIER asCALL_STDCALL>().callConv == AS_NAMESPACE_QUALIFIER asCALL_CDECL_OBJFIRST);
static_assert(as::FuncCallConv<^^call_conv::cDeclLast, AS_NAMESPACE_QUALIFIER asCALL_STDCALL>().callConv == AS_NAMESPACE_QUALIFIER asCALL_CDECL_OBJLAST);
static_assert(as::FuncCallConv<^^call_conv::stdCall, AS_NAMESPACE_QUALIFIER asCALL_STDCALL>().callConv == AS_NAMESPACE_QUALIFIER asCALL_STDCALL);
static_assert(as::FuncCallConv<^^call_conv::explicitGeneric, AS_NAMESPACE_QUALIFIER asCALL_STDCALL>().callConv == AS_NAMESPACE_QUALIFIER asCALL_GENERIC);
static_assert(as::FuncCallConv<^^call_conv::C::nonStatic, AS_NAMESPACE_QUALIFIER asCALL_STDCALL>().callConv == AS_NAMESPACE_QUALIFIER asCALL_THISCALL);
static_assert(as::FuncCallConv<^^call_conv::C::nonStaticFirst, AS_NAMESPACE_QUALIFIER asCALL_STDCALL>().callConv == AS_NAMESPACE_QUALIFIER asCALL_THISCALL_OBJFIRST);
static_assert(as::FuncCallConv<^^call_conv::C::nonStaticLast, AS_NAMESPACE_QUALIFIER asCALL_STDCALL>().callConv == AS_NAMESPACE_QUALIFIER asCALL_THISCALL_OBJLAST);
static_assert(as::FuncCallConv<^^call_conv::C::generic, AS_NAMESPACE_QUALIFIER asCALL_STDCALL>().callConv == AS_NAMESPACE_QUALIFIER asCALL_GENERIC);
static_assert(as::FuncCallConv<^^call_conv::C::Default, AS_NAMESPACE_QUALIFIER asCALL_STDCALL>().callConv == AS_NAMESPACE_QUALIFIER asCALL_STDCALL);
// Not currently supported by AngelScript:
// static_assert(as::FuncCallConv<^^call_conv::C::defaultFirst, AS_NAMESPACE_QUALIFIER asCALL_STDCALL>().callConv == AS_NAMESPACE_QUALIFIER asCALL_STDCALL);
// static_assert(as::FuncCallConv<^^call_conv::C::defaultLast, AS_NAMESPACE_QUALIFIER asCALL_STDCALL>().callConv == AS_NAMESPACE_QUALIFIER asCALL_STDCALL);
static_assert(as::FuncCallConv<^^call_conv::C::cDecl, AS_NAMESPACE_QUALIFIER asCALL_STDCALL>().callConv == AS_NAMESPACE_QUALIFIER asCALL_CDECL);
static_assert(as::FuncCallConv<^^call_conv::C::cDeclFirst, AS_NAMESPACE_QUALIFIER asCALL_STDCALL>().callConv == AS_NAMESPACE_QUALIFIER asCALL_CDECL_OBJFIRST);
static_assert(as::FuncCallConv<^^call_conv::C::cDeclLast, AS_NAMESPACE_QUALIFIER asCALL_STDCALL>().callConv == AS_NAMESPACE_QUALIFIER asCALL_CDECL_OBJLAST);
static_assert(as::FuncCallConv<^^call_conv::C::stdCall, AS_NAMESPACE_QUALIFIER asCALL_STDCALL>().callConv == AS_NAMESPACE_QUALIFIER asCALL_STDCALL);
static_assert(as::FuncCallConv<^^call_conv::C::explicitGeneric, AS_NAMESPACE_QUALIFIER asCALL_STDCALL>().callConv == AS_NAMESPACE_QUALIFIER asCALL_GENERIC);

static_assert(as::FuncCallConv<^^call_conv::generic, AS_NAMESPACE_QUALIFIER asCALL_GENERIC>().callConv == AS_NAMESPACE_QUALIFIER asCALL_GENERIC);
static_assert(as::FuncCallConv<^^call_conv::generic, AS_NAMESPACE_QUALIFIER asCALL_GENERIC>().genericType == as::GenericCallConvType::None);
static_assert(as::FuncCallConv<^^call_conv::Default, AS_NAMESPACE_QUALIFIER asCALL_GENERIC>().callConv == AS_NAMESPACE_QUALIFIER asCALL_GENERIC);
static_assert(as::FuncCallConv<^^call_conv::Default, AS_NAMESPACE_QUALIFIER asCALL_GENERIC>().genericType == as::GenericCallConvType::WrapFn);
static_assert(as::FuncCallConv<^^call_conv::defaultFirst, AS_NAMESPACE_QUALIFIER asCALL_GENERIC>().callConv == AS_NAMESPACE_QUALIFIER asCALL_GENERIC);
static_assert(as::FuncCallConv<^^call_conv::defaultFirst, AS_NAMESPACE_QUALIFIER asCALL_GENERIC>().genericType == as::GenericCallConvType::WrapObjFirst);
static_assert(as::FuncCallConv<^^call_conv::defaultLast, AS_NAMESPACE_QUALIFIER asCALL_GENERIC>().callConv == AS_NAMESPACE_QUALIFIER asCALL_GENERIC);
static_assert(as::FuncCallConv<^^call_conv::defaultLast, AS_NAMESPACE_QUALIFIER asCALL_GENERIC>().genericType == as::GenericCallConvType::WrapObjLast);
static_assert(as::FuncCallConv<^^call_conv::cDecl, AS_NAMESPACE_QUALIFIER asCALL_GENERIC>().callConv == AS_NAMESPACE_QUALIFIER asCALL_CDECL);
static_assert(as::FuncCallConv<^^call_conv::cDeclFirst, AS_NAMESPACE_QUALIFIER asCALL_GENERIC>().callConv == AS_NAMESPACE_QUALIFIER asCALL_CDECL_OBJFIRST);
static_assert(as::FuncCallConv<^^call_conv::cDeclLast, AS_NAMESPACE_QUALIFIER asCALL_GENERIC>().callConv == AS_NAMESPACE_QUALIFIER asCALL_CDECL_OBJLAST);
static_assert(as::FuncCallConv<^^call_conv::stdCall, AS_NAMESPACE_QUALIFIER asCALL_GENERIC>().callConv == AS_NAMESPACE_QUALIFIER asCALL_STDCALL);
static_assert(as::FuncCallConv<^^call_conv::explicitGeneric, AS_NAMESPACE_QUALIFIER asCALL_GENERIC>().callConv == AS_NAMESPACE_QUALIFIER asCALL_GENERIC);
static_assert(as::FuncCallConv<^^call_conv::explicitGeneric, AS_NAMESPACE_QUALIFIER asCALL_GENERIC>().genericType == as::GenericCallConvType::WrapFn);
static_assert(as::FuncCallConv<^^call_conv::C::nonStatic, AS_NAMESPACE_QUALIFIER asCALL_GENERIC>().callConv == AS_NAMESPACE_QUALIFIER asCALL_THISCALL);
static_assert(as::FuncCallConv<^^call_conv::C::nonStaticFirst, AS_NAMESPACE_QUALIFIER asCALL_GENERIC>().callConv == AS_NAMESPACE_QUALIFIER asCALL_THISCALL_OBJFIRST);
static_assert(as::FuncCallConv<^^call_conv::C::nonStaticLast, AS_NAMESPACE_QUALIFIER asCALL_GENERIC>().callConv == AS_NAMESPACE_QUALIFIER asCALL_THISCALL_OBJLAST);
static_assert(as::FuncCallConv<^^call_conv::C::generic, AS_NAMESPACE_QUALIFIER asCALL_GENERIC>().callConv == AS_NAMESPACE_QUALIFIER asCALL_GENERIC);
static_assert(as::FuncCallConv<^^call_conv::C::generic, AS_NAMESPACE_QUALIFIER asCALL_GENERIC>().genericType == as::GenericCallConvType::None);
static_assert(as::FuncCallConv<^^call_conv::C::Default, AS_NAMESPACE_QUALIFIER asCALL_GENERIC>().callConv == AS_NAMESPACE_QUALIFIER asCALL_GENERIC);
static_assert(as::FuncCallConv<^^call_conv::C::Default, AS_NAMESPACE_QUALIFIER asCALL_GENERIC>().genericType == as::GenericCallConvType::WrapMFn);
// Not supported:
// static_assert(as::FuncCallConv<^^call_conv::C::defaultFirst, AS_NAMESPACE_QUALIFIER asCALL_GENERIC>().callConv == AS_NAMESPACE_QUALIFIER asCALL_GENERIC);
// static_assert(as::FuncCallConv<^^call_conv::C::defaultLast, AS_NAMESPACE_QUALIFIER asCALL_GENERIC>().callConv == AS_NAMESPACE_QUALIFIER asCALL_GENERIC);
static_assert(as::FuncCallConv<^^call_conv::C::cDecl, AS_NAMESPACE_QUALIFIER asCALL_GENERIC>().callConv == AS_NAMESPACE_QUALIFIER asCALL_CDECL);
static_assert(as::FuncCallConv<^^call_conv::C::cDeclFirst, AS_NAMESPACE_QUALIFIER asCALL_GENERIC>().callConv == AS_NAMESPACE_QUALIFIER asCALL_CDECL_OBJFIRST);
static_assert(as::FuncCallConv<^^call_conv::C::cDeclLast, AS_NAMESPACE_QUALIFIER asCALL_GENERIC>().callConv == AS_NAMESPACE_QUALIFIER asCALL_CDECL_OBJLAST);
static_assert(as::FuncCallConv<^^call_conv::C::stdCall, AS_NAMESPACE_QUALIFIER asCALL_GENERIC>().callConv == AS_NAMESPACE_QUALIFIER asCALL_STDCALL);
static_assert(as::FuncCallConv<^^call_conv::C::explicitGeneric, AS_NAMESPACE_QUALIFIER asCALL_GENERIC>().callConv == AS_NAMESPACE_QUALIFIER asCALL_GENERIC);
static_assert(as::FuncCallConv<^^call_conv::C::explicitGeneric, AS_NAMESPACE_QUALIFIER asCALL_GENERIC>().genericType == as::GenericCallConvType::WrapMFn);

// Generic call convention in non-static class methods is not supported:
// static_assert(as::FuncCallConv<^^call_conv::C::nonStaticGeneric, AS_NAMESPACE_QUALIFIER asCALL_CDECL>().callConv == AS_NAMESPACE_QUALIFIER asCALL_GENERIC);

// clang-format on

void basic() {}

STATIC_ASSERT_EQ(as::GetFuncDecl<^^basic>(), "void basic()");

std::int8_t numeric(
    std::int16_t a,
    const std::int32_t b,
    std::int64_t& c,
    float const& d,
    double* e,
    const bool* f,
    std::uint8_t* const g,
    const std::uint64_t* const h
) {
    return 0;
}

STATIC_ASSERT_EQ(
    as::GetFuncDecl<^^numeric>(),
    // Unfortunately, type adjustment means we drop the int32 const qualifier.
    // It's not the end of the world, though. More important const qualifiers are never dropped.
    "int8 numeric(int16, int32, int64&out, const float&in, double&out, const bool&in, uint8&out, const uint64&in)"
)

AS_NAMESPACE_QUALIFIER CScriptArray* const complex[[ = as::subtype::String, = as::Auto ]](
    const AS_NAMESPACE_QUALIFIER CScriptArray*
        i[[= as::SubTypeList<as::Tmpl<AS_NAMESPACE_QUALIFIER CScriptArray, float>>()]],
    AS_NAMESPACE_QUALIFIER CScriptArray& j[[= as::subtype::String]],
    // const qualifier on k* will be lost due to type adjustment.
    AS_NAMESPACE_QUALIFIER CScriptArray* const k[[ = as::subtype::Int32, = as::Auto ]]
) {
    return nullptr;
}
// Includes "@ const+" handling in return type:
STATIC_ASSERT_EQ(
    as::GetFuncDecl<^^complex>(),
    "array<string>@ const+ complex(const array<array<float>>@, array<string>&, array<int32>@+)"
)

AS_NAMESPACE_QUALIFIER CScriptArray* complex3[[ = as::subtype::String, = as::Auto ]](
    const AS_NAMESPACE_QUALIFIER CScriptArray*
        i[[= as::SubTypeList<as::Tmpl<AS_NAMESPACE_QUALIFIER CScriptArray, float>>()]],
    AS_NAMESPACE_QUALIFIER CScriptArray& j[[= as::subtype::String]],
    // const qualifier on k* will be lost due to type adjustment.
    AS_NAMESPACE_QUALIFIER CScriptArray* const k[[ = as::subtype::Int32, = as::Auto ]]
) {
    return nullptr;
}
// Includes "@+" handling in return type:
STATIC_ASSERT_EQ(
    as::GetFuncDecl<^^complex3>(),
    "array<string>@+ complex3(const array<array<float>>@, array<string>&, array<int32>@+)"
)

AS_NAMESPACE_QUALIFIER CScriptArray* complex2[[ = as::subtype::String, = as::NonAuto ]](
    const AS_NAMESPACE_QUALIFIER CScriptArray*
        i[[= as::SubTypeList<as::Tmpl<AS_NAMESPACE_QUALIFIER CScriptArray, float>>()]],
    AS_NAMESPACE_QUALIFIER CScriptArray& j[[= as::subtype::String]],
    // const qualifier on k* will be lost due to type adjustment.
    AS_NAMESPACE_QUALIFIER CScriptArray* const k[[ = as::subtype::Int32, = as::NonAuto ]]
) {
    return nullptr;
}

STATIC_ASSERT_EQ(
    as::GetFuncDecl<^^complex2 COMMA true>(),
    "array<string>@ complex2(const array<array<float>>@+, array<string>&, array<int32>@)"
)

void defaults(
    int a,
    std::string const& s[[= as::DefVal("\"Hi\"")]] = "Hi",
    AS_NAMESPACE_QUALIFIER CScriptArray* pTest[[ = as::DefVal("null"), = as::subtype::Bool ]] = nullptr
) {}

STATIC_ASSERT_EQ(as::GetFuncDecl<^^defaults>(), "void defaults(int32, const string&in = \"Hi\", array<bool>@ = null)");

struct A {
    inline void func() const {}

    inline void func2(AS_NAMESPACE_QUALIFIER CScriptArray*) const {}
};

STATIC_ASSERT_EQ(as::GetFuncDecl<^^A::func>(), "void func() const");
STATIC_ASSERT_EQ(as::GetFuncDecl<^^A::func COMMA false COMMA true>(), "void func()");
STATIC_ASSERT_EQ(as::GetFuncDecl<^^A::func2 COMMA false COMMA true>(), "void func2(array@)");
STATIC_ASSERT_EQ(as::GetFuncDecl<^^A::func2 COMMA true COMMA true>(), "void func2(array@+)");

inline void variableParameterTypeOut(void* o, int typeId) {}

inline void variableParameterTypeIn(const void* o, int typeId) {}

inline void variableParameterTypeSurroundedByParameters(int var, void* o, int typeId, const int* var2) {}

STATIC_ASSERT_EQ(as::GetFuncDecl<^^variableParameterTypeOut>(), "void variableParameterTypeOut(?&out)");
STATIC_ASSERT_EQ(as::GetFuncDecl<^^variableParameterTypeIn>(), "void variableParameterTypeIn(const ?&in)");
STATIC_ASSERT_EQ(
    as::GetFuncDecl<^^variableParameterTypeSurroundedByParameters>(),
    "void variableParameterTypeSurroundedByParameters(int32, ?&out, const int32&in)"
);

struct PropertyAccessors {
    inline int get_prop[[= as::PropertyAccessor]]() const {
        return 6;
    }

    inline void set_prop[[= as::PropertyAccessor]](int newVal) {}

    inline int get_indexedProp[[= as::PropertyAccessor]](unsigned int idx) const {
        return 6;
    }

    inline void set_indexedProp[[= as::PropertyAccessor]](unsigned int idx, int newVal) {}
};

STATIC_ASSERT_EQ(as::GetFuncDecl<^^PropertyAccessors::get_prop>(), "int32 get_prop() const property");
STATIC_ASSERT_EQ(as::GetFuncDecl<^^PropertyAccessors::set_prop>(), "void set_prop(int32) property");
STATIC_ASSERT_EQ(
    as::GetFuncDecl<^^PropertyAccessors::get_indexedProp>(), "int32 get_indexedProp(uint32) const property"
);
STATIC_ASSERT_EQ(
    as::GetFuncDecl<^^PropertyAccessors::set_indexedProp>(), "void set_indexedProp(uint32, int32) property"
);
