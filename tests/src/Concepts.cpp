#include <angelscript.h>
#include <AngelScriptWrapper/Concepts.hpp>
#include <AngelScriptWrapper/SharedObject.hpp>
#include <AngelScriptWrapperTests/ScriptTestObject.hpp>
#include <cstdint>

static_assert(as::IsReferenceCounted<AS_NAMESPACE_QUALIFIER asIScriptEngine>);
static_assert(as::IsReferenceCounted<AS_NAMESPACE_QUALIFIER asIScriptObject>);
static_assert(as::IsReferenceCounted<as::ScriptTestObject>);
static_assert(!as::IsReferenceCounted<int>);
static_assert(!as::IsReferenceCounted<as::SharedObject<AS_NAMESPACE_QUALIFIER asIScriptEngine>>);

struct InvalidReturn {
    static inline InvalidReturn Factory() {
        return {};
    }
};

struct NotStatic {
    inline NotStatic* Factory() {
        return nullptr;
    }
};

struct InvalidName {
    static inline InvalidName* FactorY() {
        return nullptr;
    }
};

struct HasFactory {
    static inline HasFactory* Factory() {
        return nullptr;
    }
};

struct HasFactoryWithParams {
    static inline HasFactoryWithParams* Factory(int num) {
        return nullptr;
    }
};

static_assert(!as::HasFactoryFunction<std::string>);
static_assert(!as::HasFactoryFunction<InvalidReturn>);
static_assert(!as::HasFactoryFunction<NotStatic>);
static_assert(!as::HasFactoryFunction<InvalidName>);
static_assert(as::HasFactoryFunction<HasFactory>);
static_assert(as::HasFactoryFunction<HasFactoryWithParams>);

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

static_assert(as::IsConst<std::remove_reference_t<const int&>>);
static_assert(as::IsConst<const int>);
static_assert(!as::IsConst<int>);
static_assert(as::IsConst<const int* const>);
static_assert(!as::IsConst<const int*>);

static_assert(!as::IsPointer<const int>);
static_assert(!as::IsPointer<int>);
static_assert(as::IsPointer<const int* const>);
static_assert(as::IsPointer<const int*>);

static_assert(as::IsReference<int&>);
static_assert(as::IsReference<const int&>);
static_assert(!as::IsReference<int*>);
static_assert(!as::IsReference<int>);

/* Funcdefs

Function handles and delegates are received by the application as
asIScriptFunction*. We'll need a special annotation for objects of this
type called Def() or something where you give it the funcdef to use for
that instance. It will need to be applied in TypeOf<>().

How will funcdefs be declared in C++ and registered?

You might be able to do something like:

using MyFuncdef = void(int, float);

Problem: cannot attach annotations to parameters (important for SubTypeList).

Might need special struct for it that stores:
- The funcdef's name.
- The funcdef's return type, either as typename or as::Tmpl.
- The funcdef's parameter list as a SubTypes instance.

TypeOf() will need special handling for asIScriptFunction[*] that looks for the
new annotation.

GetFuncDecl() should not need special handling so long as asIScriptFunction is
annotated with as::RefType.
Function ptr C++ parameters are given the special struct.

RegisterFuncdef<SpecialStruct>();

Another problem: subtype list is handled but what if you want a funcdef that
includes another funcdef? I think it is more appropriate to store a list of
structs (for annotations) alongside the typename to cover all bases.

struct AnnotatedTypename {
    const std::meta::info type;
    StructuralSpan<const std::meta::info> annotations = {};
};

struct Funcdef {
    const AnnotatedTypename returnType;
    StructuralSpan<const AnnotatedTypename> parameters;
};

constexpr Funcdef MyFuncDefName = {
    .returnType = AnnotatedTypename(^^void),
    .parameters = [
        // Not sure handles in funcdefs can be auto, but here it is anyway:
        AnnotatedTypename(^^CScriptArray*, [as::subtype::String, as::Auto]),
        AnnotatedTypename(^^int),
    ],
};

Not sure the final C++ will be that nice, if not we need helper functions.

Unfortunately this will require a different GetFuncDecl() implementation,
and I'm not sure I can fully reuse the detail helper functions, either.
The existing GetFuncDecl() might need to construct a Funcdef object and
run the new algorithm so that I can prevent code duplication. Funcdefs
can't have default values in their parameters but we can just add a
conditional static_assert for that if we are actually bothered.

On the bright side, this does open the door to being able to generate func
declarations without the need for an actual function, which would make
writing and maintaining generic function declarations less painful (with
special annotations required for advanced use cases, should I be bothered
to add them).
*/
