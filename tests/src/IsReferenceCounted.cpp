// We'll have to test IsReferenceCounter statically, as it will impact builds otherwise.

#include <angelscript.h>
#include <AngelScriptWrapper/IsReferenceCounted.hpp>
#include <AngelScriptWrapper/Object.hpp>
#include <AngelScriptWrapperTests/ScriptTestObject.hpp>

static_assert(as::IsReferenceCounted<::asIScriptEngine>);
static_assert(as::IsReferenceCounted<::asIScriptObject>);
static_assert(as::IsReferenceCounted<as::ScriptTestObject>);

static_assert(!as::IsReferenceCounted<int>);
static_assert(!as::IsReferenceCounted<as::Object<::asIScriptEngine>>);
