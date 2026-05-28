/**
 * @file ScriptArray.hpp
 * Defines custom TypeDecls for AngelScript's array type.
 */

#pragma once

#include <AngelScriptWrapper/TypeDecl.hpp>
#include <scriptarray.h>

namespace as {
using StringArray = Specialize<AS_NAMESPACE_QUALIFIER CScriptArray, std::string>;
using StringConstArray = Specialize<const AS_NAMESPACE_QUALIFIER CScriptArray, std::string>;
using StringArrayHandle = Specialize<AS_NAMESPACE_QUALIFIER CScriptArray*, std::string>;
using ConstStringArray = Specialize<AS_NAMESPACE_QUALIFIER CScriptArray, const std::string>;
} // namespace as
