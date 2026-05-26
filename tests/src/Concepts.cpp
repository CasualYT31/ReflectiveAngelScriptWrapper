#include <angelscript.h>
#include <AngelScriptWrapper/Concepts.hpp>
#include <AngelScriptWrapper/SharedObject.hpp>
#include <AngelScriptWrapperTests/ScriptTestObject.hpp>
#include <cstdint>

static_assert(as::IsReferenceCounted<::asIScriptEngine>);
static_assert(as::IsReferenceCounted<::asIScriptObject>);
static_assert(as::IsReferenceCounted<as::ScriptTestObject>);
static_assert(!as::IsReferenceCounted<int>);
static_assert(!as::IsReferenceCounted<as::SharedObject<::asIScriptEngine>>);

static_assert(as::IsArithmetic<std::int8_t>);
static_assert(as::IsArithmetic<std::uint8_t>);
static_assert(as::IsArithmetic<std::int16_t>);
static_assert(as::IsArithmetic<std::uint16_t>);
static_assert(as::IsArithmetic<std::int32_t>);
static_assert(as::IsArithmetic<std::uint32_t>);
static_assert(as::IsArithmetic<std::int64_t>);
static_assert(as::IsArithmetic<std::uint64_t>);
static_assert(as::IsArithmetic<float>);
static_assert(as::IsArithmetic<double>);
static_assert(as::IsArithmetic<bool>);
static_assert(as::IsArithmetic<char>);
static_assert(as::IsArithmetic<const std::int8_t>);
static_assert(as::IsArithmetic<const std::uint8_t>);
static_assert(as::IsArithmetic<const std::int16_t>);
static_assert(as::IsArithmetic<const std::uint16_t>);
static_assert(as::IsArithmetic<const std::int32_t>);
static_assert(as::IsArithmetic<const std::uint32_t>);
static_assert(as::IsArithmetic<const std::int64_t>);
static_assert(as::IsArithmetic<const std::uint64_t>);
static_assert(as::IsArithmetic<const float>);
static_assert(as::IsArithmetic<const double>);
static_assert(as::IsArithmetic<const bool>);
static_assert(as::IsArithmetic<const char>);
static_assert(!as::IsArithmetic<std::int8_t*>);
static_assert(!as::IsArithmetic<std::uint8_t*>);
static_assert(!as::IsArithmetic<std::int16_t*>);
static_assert(!as::IsArithmetic<std::uint16_t*>);
static_assert(!as::IsArithmetic<std::int32_t*>);
static_assert(!as::IsArithmetic<std::uint32_t*>);
static_assert(!as::IsArithmetic<std::int64_t*>);
static_assert(!as::IsArithmetic<std::uint64_t*>);
static_assert(!as::IsArithmetic<float*>);
static_assert(!as::IsArithmetic<double*>);
static_assert(!as::IsArithmetic<bool*>);
static_assert(!as::IsArithmetic<char*>);

static_assert(as::IsConst<const int>);
static_assert(!as::IsConst<int>);
static_assert(as::IsConst<const int* const>);
static_assert(!as::IsConst<const int*>);
