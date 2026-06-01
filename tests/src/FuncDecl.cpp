#include <AngelScriptWrapper/FuncDecl.hpp>
#include <AngelScriptWrapperTests/ScriptArray.hpp>
#include <AngelScriptWrapperTests/ScriptDebugging.hpp>

void handleChecking(int a[[= as::Handle]], bool b);
constexpr auto handleCheckingParams = std::define_static_array(std::meta::parameters_of(^^handleChecking));

static_assert(as::AsHandle<handleCheckingParams[0]>());
static_assert(!as::AsHandle<handleCheckingParams[1]>());

STATIC_ASSERT_EQ(std::string(as::DefVal("5").value), "5");
STATIC_ASSERT_EQ(std::string(as::DefVal("\"5\"").value), "\"5\"");

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

AS_NAMESPACE_QUALIFIER CScriptArray* complex[[= as::subtype::String]](
    const CScriptArray* i[[= as::SubTypeList<as::Tmpl<CScriptArray, float>>()]], CScriptArray& j[[= as::subtype::String]]
) {
    return nullptr;
}

// TODO: not sure j should map to this, will need to experiment with AngelScript more.
STATIC_ASSERT_EQ(as::GetFuncDecl<^^complex>(), "array<string>@ complex(const array<array<float>>@, array<string>)")

// TODO: how should void* work?
void defaults(int a, std::string const& s[[= as::DefVal("\"Hi\"")]] = "Hi", void* pTest[[= as::DefVal("null")]] = nullptr) {}

STATIC_ASSERT_EQ(as::GetFuncDecl<^^defaults>(), "void defaults(int32, const string&in = \"Hi\", void&out = null)");

struct A {
    inline void func() const {}
};

STATIC_ASSERT_EQ(as::GetFuncDecl<^^A::func>(), "void func() const");

// TODO: https://www.angelcode.com/angelscript/sdk/docs/manual/doc_adv_var_type.html
//       How do we support this??
