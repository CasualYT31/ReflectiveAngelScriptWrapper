#include <AngelScriptWrapper/FuncDecl.hpp>
#include <AngelScriptWrapperTests/ScriptDebugging.hpp>

void handleChecking(int a[[= as::Handle]], bool b);
constexpr auto handleCheckingParams = std::define_static_array(std::meta::parameters_of(^^handleChecking));

static_assert(as::AsHandle<handleCheckingParams[0]>());
static_assert(!as::AsHandle<handleCheckingParams[1]>());

STATIC_ASSERT_EQ(std::string(as::DefVal("5").value), "5");
STATIC_ASSERT_EQ(std::string(as::DefVal("\"5\"").value), "\"5\"");

void basic() {}

STATIC_ASSERT_EQ(as::GetFuncDecl<^^basic>(), "void basic()");

// std::int8_t numeric(
//     std::int16_t a,
//     const std::int32_t b,
//     std::int64_t& c,
//     float const& d,
//     double* e,
//     const bool* f,
//     std::uint8_t* const g,
//     const std::uint64_t* const h
// ) {
//     return 0;
// }

// STATIC_ASSERT_EQ(
//     as::GetFuncDecl<^^numeric>(),
//     "int8 numeric(int16, const int32, int64&out, const float&in, double&out, const bool&in, uint8&out, const uint64&in)"
// )
