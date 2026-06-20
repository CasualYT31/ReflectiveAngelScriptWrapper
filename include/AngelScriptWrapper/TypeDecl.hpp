/**
 * @file TypeDecl.hpp
 * Defines functions that resolve typenames to their AngelScript equivalents.
 */

#pragma once

#include <angelscript.h>
#include <AngelScriptWrapper/Annotations.hpp>
#include <AngelScriptWrapper/Concepts.hpp>
#include <AngelScriptWrapper/StructuralSpan.hpp>
#include <cstdint>
#include <meta>
#include <ranges>
#include <string>
#include <type_traits>

namespace as {
/**
 * If a given C++ reflection has a Rename annotation attached to it, returns its string view.
 * Otherwise returns the C++ entity's identifier.
 *
 * For classes, you will want to use the TypeName template variable instead of this function, as this function ignores
 * any specializations of TypeName.
 * @tparam O The meta info object of the C++ entity to find the AngelScript identifier of.
 * @return The C++ entity's intended AngelScript identifier.
 */
template <std::meta::info O> consteval std::string_view GetIdentifierOf();

namespace detail {
template <typename T, SubTypes S> consteval std::string_view GetTypeDecl();
template <std::meta::info I> consteval std::string_view GetTypeDecl();
template <typename T, std::meta::info I> consteval std::string_view GetTypeDecl();

/**
 * Version of TypeOf that's used when you want to use a different type than the type of the object reflected by I.
 * Subtypes attached to I are still respected. Reserved for internal use only.
 */
template <typename T, std::meta::info I> constexpr std::string_view OverrideTypeOf = detail::GetTypeDecl<T, I>();
} // namespace detail

/**
 * Stores the AngelScript typename of the given C++ type.
 * Handles const qualifiers and pointers, though pointers are not handled recursively! This limitation is acceptable
 * since AngelScript does not allow handles to handles or references to references.
 * @tparam T The C++ type whose AngelScript type is to be deduced.
 */
template <typename T> constexpr std::string_view TypeName = as::GetIdentifierOf<^^T>();

/**
 * Stores the qualified AngelScript typename of the given C++ type that represents a template type.
 * @tparam T The base C++ type that represents a template AngelScript type.
 * @tparam S The subtypes assigned to the template type.
 * @sa SubTypeList().
 */
template <typename T, SubTypes S> constexpr std::string_view TmplTypeName = detail::GetTypeDecl<T, S>();

/**
 * Stores the qualified AngelScript typename of the given C++ entity.
 * You can provide a reflected variable, function parameter, or function to I. If a function is given, the return type
 * will be stored.
 * @tparam I The meta info object of the entity whose AngelScript type is to be stored.
 */
template <std::meta::info I> constexpr std::string_view TypeOf = detail::GetTypeDecl<I>();

/**
 * Determines if a given typename should use a handle or a reference.
 * @tparam T The type to test. This function will test the underlying type, i.e. you could pass T* here, as well.
 * @tparam C If the type is a RefType, set this flag to true to make the handle constant. Does nothing if the type is a
 *         value type.
 * @return "@" or "@ const" if the given type has been annotated with RefType, "&" otherwise.
 */
template <typename T, bool C = false> constexpr std::string_view GetRefType();

/**
 * Stores information on a single class member.
 */
struct ClassMember {
    /**
     * A reflection of the class member.
     */
    const std::meta::info member;

    /**
     * True if this member was inherited, false if it's a direct member of the class.
     */
    const bool inherited;
};

/**
 * The result of a call to GetClassHierarchy().
 */
struct ClassInformation {
    /**
     * A reflection of the type whose members are stored in this struct.
     */
    std::meta::info type;

    /**
     * The base classes, both direct and indirect, of type, if any.
     */
    const StructuralSpan<const std::meta::info> bases;

    /**
     * The known derived classes, both direct and indirect, of type, if any.
     */
    const StructuralSpan<const std::meta::info> derivatives;

    /**
     * The members of type.
     * Check the documentation on GetClassHierarchy() to see which members will be
     */
    const StructuralSpan<const ClassMember> members;
};

/**
 * Retrieves the direct and inherited public members, and the direct and indirect bases, of a given class.
 * There are a variety of members this function will not include in the result:
 * - Any private or protected members.
 * - Any members of base classes with protected or private inheritance.
 * - Any constructors.
 * - Any destructors.
 * - Any special member functions.
 * - Any embedded types.
 * - Any members that do not have an identifier, <em>except for</em> operator functions.
 *
 * Though the order of the results is defined (e.g. members are always stored in declaration order, with special member
 * functions coming last), you should not rely on it. In a similar fashion, you should not rely on each instance of
 * std::meta::info within the members field belonging to a certain class. You can only rely on the fact that members can
 * be accessed via type.
 *
 * This function does not distinguish between different versions of overridden members (this includes fields, so don't
 * override those!): it will simply return the first version found. It does, however, include all function overloads.
 *
 * Note that no derived classes will be recorded for C's entry, as there is no way for the function to know which
 * classes have derived from a given class. We can know the derived classes of each base class, though, at least those
 * within the hierarchy from C, so these will be recorded in separate entries if recurse is true.
 *
 * If a class has the DoNotRegister annotation attached to it, it will be entirely excluded from the result, along with
 * all of its base classes. Base classes could still be included in the result of a different call to
 * GetClassHierarchy().
 *
 * If a class member has the DoNotRegister annotation attached to it, it will be entirely excluded from the result.
 *
 * If a base class has the Mixin annotation attached to it, derived classes will inherit all of their members, but the
 * derived classes will not treat the Mixin as a base class, and the Mixin class will not be included as a separate
 * entry if recursion is enabled. This includes any base classes of the Mixin class itself. They will only be treated as
 * actual base classes if they are accessed via a non-Mixin context in the hierarchy.
 * @tparam C The reflection of the class you wish to traverse. It is forbidden for Mixin to be attached to C.
 * @param recurse By default, only the given class is traversed. If true is given, however, each separate base class
 *        will also be traversed in the same manner.
 * @return C's information will be stored in the first element of the span always. Further elements will be added
 *         depending on the value of recurse.
 */
template <std::meta::info C>
consteval StructuralSpan<const ClassInformation> GetClassHierarchy(const bool recurse = false);
} // namespace as

#include <AngelScriptWrapper/TypeDecl.tpp>

namespace as {
/**
 * Contains commonly-used subtype lists.
 * Application developers are encouraged to define their own commonly-used subtype lists for reuse.
 */
namespace subtype {
constexpr auto Int8 = SubTypeList<std::int8_t>();
constexpr auto Uint8 = SubTypeList<std::uint8_t>();
constexpr auto Int16 = SubTypeList<std::int16_t>();
constexpr auto Uint16 = SubTypeList<std::uint16_t>();
constexpr auto Int32 = SubTypeList<std::int32_t>();
constexpr auto Uint32 = SubTypeList<std::uint32_t>();
constexpr auto Int64 = SubTypeList<std::int64_t>();
constexpr auto Uint64 = SubTypeList<std::uint64_t>();
constexpr auto Float = SubTypeList<float>();
constexpr auto Double = SubTypeList<double>();
constexpr auto Bool = SubTypeList<bool>();
constexpr auto String = SubTypeList<std::string>();
} // namespace subtype
} // namespace as
