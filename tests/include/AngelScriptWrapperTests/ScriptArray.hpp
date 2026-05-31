/**
 * @file ScriptArray.hpp
 * Defines custom TypeDecls for AngelScript's array type.
 */

#pragma once

#include <AngelScriptWrapper/TypeDecl.hpp>
#include <scriptarray.h>

namespace as {
constexpr auto StringArray = SubType<std::string>();
constexpr auto ConstStringArray = SubType<const std::string>();
constexpr auto StringHandleArray = SubType<std::string*>();
constexpr auto ConstStringHandleArray = SubType<const std::string*>();
constexpr auto ConstStringConstHandleArray = SubType<const std::string* const>();
} // namespace as
