/**
 * @file Annotations.hpp
 * Defines the annotations used throughout the library, alongside helper functions for annotations.
 */

#pragma once

#include <angelscript.h>
#include <AngelScriptWrapper/Concepts.hpp>
#include <AngelScriptWrapper/StructuralSpan.hpp>
#include <meta>
#include <optional>
#include <span>
#include <string_view>

namespace as {

// MARK: Helpers

/**
 * Gets a list of annotations attached to the given C++ entity, that match the given annotation type.
 * @tparam I The C++ entity to get annotations from.
 * @tparam T Annotations must be of this type to be included in the resulting list.
 * @return The list of T annotations on I.
 */
template <std::meta::info I, typename T> consteval std::span<const std::meta::info> GetAnnotations();

/**
 * Attempt to extract a single annotation of type T from I.
 * This function will fail compilation if more than one annotation of type T can be found on I.
 * @tparam I The C++ entity to extract the annotation from.
 * @tparam T The type of annotation to extract from I.
 * @return The extracted annotation, if found. std::nullopt if no annotations of type T were attached to I.
 */
template <std::meta::info I, typename T> constexpr std::optional<T> ExtractAnnotation();

/**
 * Finds out if at least one annotation of type T is attached to I.
 * @tparam I The C++ entity to search through.
 * @tparam T The type of annotation to search for.
 * @return True if at least one annotation of type T is attached to I, false otherwise.
 */
template <std::meta::info I, typename T> constexpr bool HasAnnotation();

/**
 * Version of GetAnnotations() that accepts T in meta info object form.
 * @sa GetAnnotations<std::meta::info, typename>()
 */
template <std::meta::info I, std::meta::info T> consteval std::span<const std::meta::info> GetAnnotations() {
    return GetAnnotations<I, typename[:T:]>();
}

/**
 * Version of ExtractAnnotation() that accepts T in meta info object form.
 * @sa ExtractAnnotation<std::meta::info, typename>()
 */
template <std::meta::info I, std::meta::info T> constexpr std::optional<typename[:T:]> ExtractAnnotation() {
    return ExtractAnnotation<I, typename[:T:]>();
}

/**
 * Version of HasAnnotation() that accepts T in meta info object form.
 * @sa HasAnnotation<std::meta::info, typename>()
 */
template <std::meta::info I, std::meta::info T> constexpr bool HasAnnotation() {
    return HasAnnotation<I, typename[:T:]>();
}

/**
 * Finds out if there is only zero or one true values in a list of bools.
 * @tparam Bs The bools to test.
 * @return True if there is only zero or one trues in Bs, false otherwise.
 */
template <bool... Bs> consteval bool AtLeastOneOf();

// MARK: Const

/**
 * Annotation attached to global properties to denote that they should be const-qualified.
 * AngelScript does not allow you to register global properties using const-qualified C++ objects, meaning we can't
 * reflect on the C++ const qualifier to tell whether a global property should be const or not. If you want to make a
 * global property const, you will need to assign the corresponding C++ object this annotation.
 * @sa EngineOptions::ConstGlobalPropertiesDefault
 */
inline constexpr struct {
} GlobalConst{};

/**
 * Annotation attached to global properties to denote that they should not be const-qualified.
 * Only useful if you change the ConstGlobalPropertiesDefault engine option, as the default is to make global properties
 * non-const already.
 * @sa EngineOptions::ConstGlobalPropertiesDefault
 */
inline constexpr struct {
} GlobalNotConst{};

/**
 * Finds out if the developer has marked a global property as being const-qualified in AngelScript.
 * @tparam O The object to test.
 * @tparam Fallback If the object has no explicit const-related annotation, then use this fallback.
 * @return True if the global property should be const-qualified in the application interface, false if not.
 */
template <std::meta::info O, bool Fallback> constexpr bool IsGlobalConstQualified();

// MARK: Rename

/**
 * Annotation attached to classes, functions, and variables to set their names in the AngelScript interface.
 * Usually, the identifier of a C++ entity registered with the application interface will be used as its corresponding
 * name in AngelScript. If you want to change this name, then attach this annotation. Note that this will not override a
 * class's TypeName specialization.
 */
struct Rename {
    const char* to;
};

/**
 * Used to attach a Rename annotation to a C++ entity.
 * @param n The name to give to the C++ entity in the AngelScript application interface.
 * @return The annotation to attach to the C++ entity.
 */
inline consteval Rename Name(std::string_view n) {
    return Rename{ std::define_static_string(n) };
}

// MARK: Auxiliary Objects

/**
 * Annotation attached to functions that require an auxiliary pointer.
 * The pointer itself is runtime information, so it can't be attached to functions at compile time. Instead, we need to
 * attach a compile time label to the function so that when the Engine wrapper encounters it, it can pull from its
 * runtime-defined auxiliary object map, using said label as the key.
 */
struct AuxiliaryLabel {
    const char* to;
};

/**
 * Used to attach an AuxiliaryLabel annotation to a C++ function.
 * @param label The auxiliary label to give to the C++ function.
 * @return The annotation to attach to the C++ function.
 */
inline consteval AuxiliaryLabel AuxLabel(std::string_view label) {
    return AuxiliaryLabel{ std::define_static_string(label) };
}

// MARK: SubTypes

/**
 * Annotation attached to variables, fields and function parameters that stores a list of subtypes used in an
 * AngelScript template specialization.
 * If it is attached to a function, the subtypes will be applied to the function's return type.
 * Both vectors are guaranteed to be equal in length, even if entries in recursiveSub are empty.
 * @sa SubTypeList()
 */
struct SubTypes {
    /**
     * A list of meta info objects for each subtype, from left to right.
     */
    StructuralSpan<const std::meta::info> subTypes;

    /**
     * If a subtype in the list itself has subtypes, they must be assigned to the corresponding element in this list.
     */
    StructuralSpan<const SubTypes> recursiveSub;
};

/**
 * Use this struct in the template parameter list of SubTypeList() when you need to declare a template type whose
 * subtypes are also template types.
 * @tparam B The C++ type that represents the AngelScript template type.
 * @tparam S The subtypes to instantiate the template type with.
 */
template <typename B, typename... S> struct Tmpl {};

/**
 * Used to generate a SubTypes annotation at compile time.
 * @tparam Ts... The subtypes of a template type instance.
 * @return The annotation to attach to e.g. a CScriptArray variable, parameter or return value to tell the library which
 *         specialization you expect from scripts.
 * @sa Tmpl
 */
template <typename... Ts> consteval SubTypes SubTypeList();

// MARK: RefType

/**
 * Annotation attached to classes to denote that they are reference types.
 * If a class is not annotated with this object, the library assumes it is a value type.
 */
inline constexpr struct {
} RefType{};

/**
 * Finds out if the developer has marked a C++ type as being a reference type in AngelScript.
 * @tparam T The type to test. Aliases are dealised. The base type will be tested, e.g. if T* is passed, the pointer
 *         will be dropped. CV qualifiers are also dropped.
 * @return True if the type has been marked as a ref type, false if the type is a value type.
 */
template <typename T> constexpr bool IsRefType();

// MARK: Auto Handles

/**
 * Annotation attach to function parameters to tell the library that it should be registered as an auto handle.
 * This annotation will be ignored if attached to a value type, or a reference type that isn't a pointer.
 * @sa EngineOptions::AutoHandleDefault.
 */
inline constexpr struct {
} Auto{};

/**
 * Annotation attach to function parameters to tell the library that it should be registered as a regular handle (i.e.,
 * not as an auto handle).
 * This annotation will be ignored if attached to a value type, or a reference type that isn't a pointer.
 * @sa EngineOptions::AutoHandleDefault.
 */
inline constexpr struct {
} NonAuto{};

// MARK: Default Values

/**
 * Annotation attached to function parameters that are to be registered with a default value.
 * Compilation will fail if you attach more than one of these to a single parameter.
 */
struct DefaultsTo {
    /**
     * The default value to render into an AngelScript function parameter declaration, verbatim.
     */
    const char* value;
};

/**
 * Use this helper function to assign a default value to a function parameter in AngelScript.
 * @param val The value to render into the function declaration, verbatim. This means that if you want to assign a
 *        string default value, you must surround the value with double quotes!
 * @return The annotation to apply to the parameter.
 */
inline consteval DefaultsTo DefVal(std::string const& val) {
    return DefaultsTo{ std::define_static_string(val) };
}

// MARK: Call Convention

/**
 * Annotation attached to functions to tell the library that it uses the CDecl call convention.
 * Trying to attach this annotation to ineligible functions will cause a compiler error (e.g. attaching to non-static
 * class methods, attaching to functions that use the generic call convention, etc.).
 * @sa EngineOptions::CallConventionDefault.
 */
inline constexpr struct {
} CDecl{};

/**
 * Annotation attached to functions to tell the library that it uses the StdCall call convention.
 * Trying to attach this annotation to ineligible functions will cause a compiler error (e.g. attaching to non-static
 * class methods, attaching to functions that use the generic call convention, etc.).
 * @sa EngineOptions::CallConventionDefault.
 */
inline constexpr struct {
} StdCall{};

/**
 * Annotation attached to functions to tell the library that it uses the StdCall call convention.
 * You have two options when it comes to registering functions that use the generic calling convention:
 * <ol>
 *  <li>Writing the function as you normal would with CDecl/StdCall/etc. functions, but attaching this annotation
 *      <u>without</u> a declaration. This will tell the library to generate the declaration for you, and one of the
 *      WRAP macros provided by AngelScript will be used to generate the required wrapper function that accepts an
 *      asIScriptGeneric* parameter. You can also combine this type of the GenericWithDecl annotation with the ObjFirst
 *      and ObjLast annotations to tell the library to use the relevant wrapper macros.</li>
 *  <li>Writing the function with the following signature: void(asIScriptGeneric*). Then, attaching this annotation
 *      <u>with</u> a <u>full</u> AngelScript function declaration. This explicit declaration is required as there is no
 *      way for this library to detect which declaration you intend otherwise. You will then of course need to manage
 *      accessing parameters and setting the return value manually.</li>
 * </ol>
 *
 * Trying to attach this annotation to ineligible functions will cause a compiler error (e.g. attaching to non-static
 * class methods, etc.).
 * @sa EngineOptions::CallConventionDefault.
 * @sa Generic().
 */
struct GenericWithDecl {
    const char* decl;
};

/**
 * Used to attach a GenericWithDecl annotation to a C++ function.
 * @param d The AngelScript declaration to register the function with. If one is not given, the library will try to
 *        generate one for you (i.e., this will only work if you don't explicitly accept an asIScriptGeneric*
 *        parameter).
 * @return The generic call convention annotation.
 */
inline consteval GenericWithDecl Generic(std::string_view d = "") {
    return GenericWithDecl{ std::define_static_string(d) };
}

/**
 * Annotation attached to functions to tell the library that it uses the _OBJFIRST variant of whatever calling
 * convention is determined.
 */
inline constexpr struct {
} ObjFirst{};

/**
 * Annotation attached to functions to tell the library that it uses the _OBJLAST variant of whatever calling
 * convention is determined.
 */
inline constexpr struct {
} ObjLast{};

/**
 * Indicates which type of wrap function to generate for a function which is to be registered with the generic call
 * convention.
 */
enum class GenericCallConvType {
    /**
     * No macro should be used, for functions that have the void(asIScriptGeneric*) signature.
     */
    None,

    /**
     * Maps to WRAP_FN, used for regular functions.
     */
    WrapFn,

    /**
     * Maps to WRAP_MFN, used for static class functions.
     */
    WrapMFn,

    /**
     * Maps to WRAP_OBJ_FIRST, used for AngelScript object methods (the pointer to the object will be passed as the
     * first parameter).
     */
    WrapObjFirst,

    /**
     * Maps to WRAP_OBJ_LAST, used for AngelScript object methods (the pointer to the object will be passed as the last
     * parameter).
     */
    WrapObjLast,
};

// MARK: Property Accessors

/**
 * Annotation attached to functions that are to be property accessors.
 * The identifier of the function must be prepended with get_ or set_ according to which type of accessor you wish to
 * declare.
 */
inline constexpr struct {
} PropertyAccessor{};

// MARK: Funcdefs

/**
 * Annotations attached to asIScriptFunction or asIScriptFunction* objects that tells the library which funcdef it
 * should use.
 */
struct Funcdef {
    /**
     * A reflection of the C++ function declaration to pull the funcdef name from.
     */
    const std::meta::info funcdef;
};

// MARK: Exclusions

/**
 * Annotation attached to classes and class members that should not be registered with the application interface.
 */
inline constexpr struct {
} DoNotRegister{};

/**
 * Annotation attached to base classes that should be treated as mixins during GetClassHierarchy() calls.
 * This means that derived classes will inherit all of their members, but the class itself will not be treated as a base
 * class of the derived class, and thus will not be registered in the application interface separately. This includes
 * all base classes of the Mixin class, even if they are not marked with the Mixin annotation.
 */
inline constexpr struct {
} Mixin{};

// MARK: Interfaces

/**
 * Annotations attached to asIScriptObject or asIScriptObject* objects that tells the library which interface it is
 * expected to implement.
 */
struct Interface {
    /**
     * A reflection of the C++ interface to pull the interface name from.
     */
    const std::meta::info interface;
};

// MARK: Behaviours

/**
 * Annotation attached to a non-static or static method to tell RegisterObjectType() that it is intended to be
 * registered with the stored behaviour.
 */
struct Behaviour {
    /**
     * The asEBehaviours code to register the method with.
     */
    const AS_NAMESPACE_QUALIFIER asEBehaviours code;
};

/**
 * Tests a function to see if it has been marked with the given behaviour code.
 * @tparam F The function to test.
 * @tparam C The behaviour code to search for.
 * @return True if the function has been marked with the given behaviour code, false otherwise.
 */
template <std::meta::info F, AS_NAMESPACE_QUALIFIER asEBehaviours C> inline constexpr bool IsBehaviour() {
    constexpr auto behaviour = ExtractAnnotation<F, Behaviour>();
    return behaviour && behaviour->code == C;
}
} // namespace as

#include <AngelScriptWrapper/Annotations.tpp>
