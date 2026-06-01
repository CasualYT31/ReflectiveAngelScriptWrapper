/**
 * @file ScriptArray.hpp
 * Defines custom TypeDecls for AngelScript's array type.
 */

#pragma once

#include <AngelScriptWrapper/TypeDecl.hpp>
#include <scriptarray.h>

namespace as {
constexpr auto ConstStringArray = SubTypeList<const std::string>();
constexpr auto StringHandleArray = SubTypeList<std::string*>();
constexpr auto ConstStringHandleArray = SubTypeList<const std::string*>();
constexpr auto ConstStringConstHandleArray = SubTypeList<const std::string* const>();
} // namespace as
