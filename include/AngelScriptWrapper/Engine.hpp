/**
 * @file Engine.hpp
 * Defines the wrapper class for an asIScriptEngine instance.
 */

#pragma once

#include <angelscript.h>
#include <AngelScriptWrapper/FuncDecl.hpp>
#include <AngelScriptWrapper/Object.hpp>
#include <AngelScriptWrapper/OwnedObject.hpp>
#include <AngelScriptWrapper/SharedObject.hpp>
#include <assert.h>
#include <memory>
#include <meta>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>

namespace as {
/**
 * The type that represents auxiliary labels at runtime.
 */
using AuxiliaryKey = std::string;

/**
 * In parameter form of AuxiliaryKey.
 */
using AuxiliaryKeyIn = AuxiliaryKey const&;

/**
 * Generic pointer to an auxiliary object.
 */
using AuxiliaryPtr = void*;

/**
 * Maps auxiliary labels to auxiliary pointers.
 */
using AuxiliaryMap = std::unordered_map<AuxiliaryKey, AuxiliaryPtr>;

struct EngineOptions {
    /**
     * Set this to true if you want all global properties to be const-qualified by default.
     */
    bool ConstGlobalPropertiesDefault = false;

    /**
     * The default call convention to use if a given function's calling convention can't be deduced.
     * Can only be one of asCALL_CDECL, asCALL_STDCALL or asCALL_GENERIC. _OBJFIRST and _OBJLAST will still be used
     * where requested via annotations.
     *
     * Usually, Engine will be able to deduce a function's call convention via reflection. E.g. if a function is a
     * non-static member of a class, asCALL_THISCALL or some variation of it will always be used. AngelScript's generic
     * call convention can also be very easy to deduce via reflection. But it is not currently possible via reflection
     * to tell when CDecl or StdCall is being used, so the decision was made to introduce two features:
     * as::CDecl/as::StdCall annotations (to explicitly mark functions as using one call convention), and this option
     * (which, for most people, eliminates the need to explicitly annotate functions with CDecl or StdCall at all).
     *
     * One caveat when setting this to asCALL_GENERIC: it assumes that functions without any call convention-based
     * annotations are written without the void(asIScriptGeneric*) signature. If they are, then you will still need to
     * explicitly mark the function as generic using the Generic() function, making sure to pass the full AngelScript
     * function declaration to Generic(), otherwise the Engine wrapper won't know which declaration to use.
     */
    AS_NAMESPACE_QUALIFIER asDWORD CallConventionDefault = AS_NAMESPACE_QUALIFIER asCALL_CDECL;

    /**
     * Set this to true if you wish to make handle parameters auto handles by default.
     */
    bool AutoHandleDefault = false;
};

/**
 * A wrapper around an asIScriptEngine instance.
 * @tparam Opts Compile-time options to apply to the Engine wrapper.
 */
template <EngineOptions Opts = EngineOptions{}> struct Engine {
    /**
     * Initializes a new asIScriptEngine instance inside a wrapper object.
     */
    inline Engine() {
        m_engine = std::make_unique<OwnedObject<AS_NAMESPACE_QUALIFIER asIScriptEngine>>(
            AS_NAMESPACE_QUALIFIER asCreateScriptEngine()
        );
    }

    /**
     * Initializes an engine wrapper with an existing asIScriptEngine instance.
     * @warning Usually this wrapper object will release the engine when the wrapper object is destructed, but if the
     *          wrapper is constructed in this way, then the engine will not be released and it is the responsibility of
     *          the caller to release the engine that it provides here.
     */
    inline Engine(AS_NAMESPACE_QUALIFIER asIScriptEngine* const engine) {
        m_engine = std::make_unique<SharedObject<AS_NAMESPACE_QUALIFIER asIScriptEngine>>(engine);
    }

    Engine(Engine&&) = delete;
    Engine& operator=(Engine&&) = delete;

    /**
     * Grants access to the underlying scripting engine instance.
     * @warning If this wrapper owns the returned engine instance, then the returned pointer will become invalid once
     *          the wrapper is destructed.
     * @return Pointer to the scripting engine.
     */
    inline AS_NAMESPACE_QUALIFIER asIScriptEngine* Ptr() const noexcept {
        return m_engine->Ptr();
    }

    /**
     * Finds out if this wrapper has a scripting engine stored within it.
     * @return True if this wrapper has a scripting engine, false if not.
     */
    inline bool HasEngine() const noexcept {
        return m_engine && *m_engine;
    }

    // MARK: Auxiliary Map

    /**
     * Replace the auxiliary object map.
     * @param newMap The new map.
     */
    inline void SetAuxiliaryMap(AuxiliaryMap const& newMap) {
        m_auxMap = newMap;
    }

    /**
     * Adds an auxiliary object to the wrapper's map.
     * If a global function or object behaviour or method requires an auxiliary pointer, you are to configure one using
     * this method or one of its equivalents. First, you give the pointer to the object, along with a unique label,
     * before trying to register the thing that requires said pointer. Then, you use AuxLabel() to attach an
     * AuxiliaryLabel annotation to the function that will be registered. The label in the annotation must match the one
     * given to this method. The relevant registration method will then pull the auxiliary pointer from the map and add
     * it to the underlying AngelScript engine registration call.
     * @warning This method asserts that an auxiliary object with the given label wasn't already added.
     * @param k The auxiliary label to give to the object.
     * @param v A pointer to the auxiliary object.
     */
    void AddAuxiliaryObject(AuxiliaryKeyIn k, AuxiliaryPtr const v);

    /**
     * Variant of AddAuxiliaryObject that lets you add multiple objects.
     * @tparam Ts The types of the rest of the label-pointer pairs, if any.
     * @param k The auxiliary key to give to the first object.
     * @param v A pointer to the first auxiliary object.
     * @param kvs The rest of the label-pointer pairs, if any.
     */
    template <typename... Ts> void AddAuxiliaryObjects(AuxiliaryKeyIn k, AuxiliaryPtr const v, Ts... kvs);

    /**
     * Gets the number of auxiliary objects that are currently in the wrapper's map.
     * @return The number of auxiliary objects currently added.
     */
    inline std::size_t GetAuxiliaryObjectCount() const noexcept {
        return m_auxMap.size();
    }

    /**
     * Pulls a previously-added auxiliary object from the wrapper's map.
     * @param k The label of the object to pull.
     * @return The pointer to the auxiliary object.
     */
    inline AuxiliaryPtr GetAuxiliaryObject(AuxiliaryKeyIn k) const {
        return m_auxMap.at(k);
    }

    // MARK: Global Properties

    /**
     * Registers a global property for use in the scripts within this engine.
     * The name of the global property in the application interface is derived from the identifier of the given C++
     * object. If you want to change this name, you will need to attach a Rename annotation to the C++ object via the
     * use of the Name() function.
     * @tparam V The std::meta::info object of your global variable of type T.
     * @tparam T The type of global property to register. AngelScript does not allow registering global properties
     *         linked to const C++ objects. If you want to register a constant global property, attach the GlobalConst
     *         annotation to the object pointed to by the value parameter. Conversely, if you need to explicitly mark a
     *         global property as being not const-qualified, then attach the GlobalNotConst annotation.
     * @param value Pointer to the value to give to the global property. You are expected to manage the global
     *        property's value as you would without this wrapper.
     * @return The result of the registration.
     */
    template <std::meta::info V, typename T>
        requires(!IsConst<T>)
    int RegisterGlobalProperty(T* const value);

    /**
     * Registers a global property for use in the scripts whose pointer is being managed by an OwnedObject wrapper.
     * The name of the global property in the application interface is derived from the identifier of the given
     * OwnedObject instance. If you want to change this name, you will need to attach a Rename annotation to the
     * OwnedObject instance via the use of the Name() function.
     * @tparam V The std::meta::info object of your OwnedObject wrapper. This means that if your underlying object is of
     *         a specialized template type, you must attach SubTypes annotations to the Object wrapper!
     * @tparam T The type of global property to register. AngelScript does not allow registering global properties
     *         linked to const C++ objects. If you want to register a constant global property, attach the GlobalConst
     *         annotation to the OwnedObject instance. Conversely, if you need to explicitly mark a global property as
     *         being not const-qualified, then attach the GlobalNotConst annotation.
     * @param value The OwnedObject wrapper that manages the AngelScript object to use as the value of the global
     *        property. The underlying pointer is used so make sure this OwnedObject survives for as long as the script
     *        engine is running.
     * @return The result of the registration.
     */
    template <std::meta::info V, typename T>
        requires(!IsConst<T>)
    int RegisterGlobalProperty(OwnedObject<T> const& value);

    // MARK: Global Functions

    /**
     * Registers a global function for use in the scripts within this engine.
     * Note that you aren't able to register overloads of functions directly. You will need to use FindOverload()
     * beforehand and use the return of that function.
     *
     * If you need to provide an auxiliary object, such as when registering a non-static class method, you will need to
     * add it using AddAuxiliaryObject() beforehand, and then attach an AuxiliaryLabel annotation to the non-static
     * class method with the same label you gave to the AddAuxiliaryObject() call.
     * @tparam F The reflection of the function to register as a global function in the application interface.
     * @return The result of the registration.
     * @sa Annotations.hpp
     */
    template <std::meta::info F> int RegisterGlobalFunction();

    /**
     * Version of RegisterGlobalFunction that writes the call convention used into an out reference.
     * Primarily used for testing.
     */
    template <std::meta::info F> int RegisterGlobalFunction(AS_NAMESPACE_QUALIFIER asDWORD& callConvOut);

    // MARK: Funcdefs

    /**
     * Registers a funcdef for use in the scripts within this engine.
     * asIScriptFunction* parameters and objects can then be given the Funcdef(F) annotation to tell Engine which type
     * of function handle to work with.
     * @tparam F A reflection of the C++ function declaration to generate a funcdef for. The name of the function will
     *         become the name of the funcdef.
     * @return The result of the registration.
     */
    template <std::meta::info F> int RegisterFuncdef();

    // MARK: Typedefs

    /**
     * Registers a typedef for use in the scripts within this engine.
     * Make sure to pass the typedef you intend! The typedef will be recursively dealiased to find the type you intend
     * to register the typedef with, but the name of the typedef will be based on the _first_ alias in the chain (and
     * whether it has a Rename annotation attached to it or not).
     * @tparam T A reflection of the typedef to register.
     * @return The result of the registration.
     */
    template <std::meta::info T> int RegisterTypedef();

    // MARK: Enums

    /**
     * Registers an enum and all of its values for use in the scripts within this engine.
     * Note that each value of the given enum will be cast to an int as this is the type AngelScript expects. Also note
     * that you can rename the enum, as well as individual values, with the Rename annotation.
     * @tparam E A reflection of the enum to register.
     * @return The type ID of the enum on success, or the first error code encountered on failure.
     */
    template <std::meta::info E> int RegisterEnum();

    // MARK: Interfaces

    /**
     * Registers a C++ interface for use in the scripts within this engine.
     * The first thing to note is that this function will only register public, pure virtual methods of the given class,
     * that aren't operator functions, constructors or destructors.
     *
     * This function will make sure that the registered interface not only has the pure virtual methods of the given
     * class, but all of its base classes recursively, too. You can optionally register each base class as a separate
     * interface in the same way by setting R to true.
     *
     * Multiple methods that share the same name and signature inherited from different classes using multiple
     * inheritance will be ignored in favour of the first method found in the registered interface.
     * @warning This function will not take C++ namespaces into account. If more than one class in I's inheritance chain
     *          has the same identifier, you will need to rename one or more of them using as::Name() to avoid an error
     *          code of asALREADY_REGISTERED.
     * @tparam I A reflection of the interface to register.
     * @tparam R Whether or not to recursively register each base interface separately, too.
     * @return >= 0 on success, or the first error code encountered on failure.
     */
    template <std::meta::info I, bool R = true> int RegisterInterface();

    /**
     * Figures out if this wrapper has already been used to register the given interface.
     * This method will only return true if the RegisterInterface() template method has been used to register the given
     * interface. If the interface has been registered directly via Ptr(), then this method will always return false.
     * @tparam I The interface to query.
     * @return True if the interface has already been registered using RegisterInterface(), false if not.
     */
    template <typename I> inline bool HasRegisteredInterface() {
        return m_interfaces.contains(std::type_index(typeid(I)));
    }

    // MARK: Object Types

    /**
     * Registers a C++ type for use in the scripts within this engine.
     * Please read the documentation on [reference types](/readmes/06-REFERENCE-TYPES.md) and
     * [value types](/readmes/10-VALUE-TYPES.md) for more information.
     * @tparam T A reflection of the type to register.
     * @tparam R Whether or not to recursively register each base type separately, too. If so, casting operators will be
     *         registered on each class in the hierarchy to establish their relationships. If a casting operator has
     *         already been registered, it will be skipped for you. This goes for types in general, too.
     * @return The type ID of T on success, or the first error code encountered on failure.
     */
    template <std::meta::info T, bool R = true> int RegisterObjectType();

    /**
     * Figures out if this wrapper has already been used to register the given type.
     * This method will only return true if the RegisterObjectType() template method has been used to register the given
     * type. If the type has been registered directly via Ptr(), then this method will always return false.
     * @tparam T The type to query.
     * @return True if the type has already been registered using RegisterObjectType(), false if not.
     */
    template <typename T> inline bool HasRegisteredObjectType() {
        return m_types.contains(std::type_index(typeid(T)));
    }

    /**
     * You will need to inform the engine wrapper about POD value types that have been registered externally, otherwise
     * POD type checking will not work correctly.
     * You can use this method to add said types to the internal POD value type list. For example, after registering the
     * CDateTime or Complex add on types, you must invoke Engine<Opts>::AddPodType<CDateTime>() or
     * Engine<Opts>::AddPodType<Complex>() respectively.
     *
     * POD type checking will break if you pass a type that isn't a POD value type!
     *
     * You do not need to call this method for POD value types you register via RegisterObjectType().
     * @tparam T The type to add.
     */
    template <typename T> inline void AddPodType() {
        m_podTypes.insert(std::type_index(typeid(T)));
    }

    /**
     * Checks if a type registered via this engine wrapper is a POD value type.
     * @tparam T The type to check.
     * @return True if the given type was either registered by the wrapper as a POD value type, or was previously added
     *         to the list of known POD value types via a call to AddPodType().
     * @sa AddPodType()
     */
    template <typename T> inline bool IsPodType() {
        return m_podTypes.contains(std::type_index(typeid(T)));
    }

private:
    /**
     * Points to the AngelScript engine instance this wrapper interfaces with.
     * Becomes either Owned or Shared depending on how the Engine was constructed.
     */
    std::unique_ptr<Object<AS_NAMESPACE_QUALIFIER asIScriptEngine>> m_engine;

    /**
     * Maps auxiliary labels to auxiliary objects.
     */
    AuxiliaryMap m_auxMap;

    /**
     * Keeps track of which interfaces the wrapper's already registered.
     * This is used to stop the wrapper from trying to register the same interface more than once.
     */
    std::unordered_set<std::type_index> m_interfaces;

    /**
     * Keeps track of which types the wrapper's already registered.
     * This is used to stop the wrapper from trying to register the same type more than once. However, it does not stop
     * new casting operators from being registered for existing types.
     */
    std::unordered_set<std::type_index> m_types;

    /**
     * Keeps track of which casting operators have been registered for which object types.
     * This is used to prevent asALREADY_REGISTERED errors when recursively registering two or more classes that inherit
     * from the same base class/es.
     */
    std::unordered_map<std::string, std::unordered_set<std::string>> m_castingOperators;

    /**
     * Keeps track of the POD value types that the wrapper knows of.
     * Is prepopulated with the primitive types AngelScript supports.
     */
    std::unordered_set<std::type_index> m_podTypes = {
        std::type_index(typeid(bool)),          std::type_index(typeid(std::int8_t)),
        std::type_index(typeid(std::uint8_t)),  std::type_index(typeid(std::int16_t)),
        std::type_index(typeid(std::uint16_t)), std::type_index(typeid(std::int32_t)),
        std::type_index(typeid(std::uint32_t)), std::type_index(typeid(std::int64_t)),
        std::type_index(typeid(std::uint64_t)), std::type_index(typeid(float)),
        std::type_index(typeid(double)),
    };
};
} // namespace as

#include <AngelScriptWrapper/Engine.tpp>
