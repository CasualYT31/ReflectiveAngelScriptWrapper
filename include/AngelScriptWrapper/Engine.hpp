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
#include <memory>
#include <meta>

namespace as {
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
     * @tparam F The reflection of the function to register as a global function in the application interface.
     * @param auxiliary Passed directly to the asIScriptEngine::RegisterGlobalFunction() method. TODO: currently this is
     *        only passed when using the THISCALL_ASGLOBAL call convention, not sure how this is used with the generic
     *        call convention? https://www.angelcode.com/angelscript/sdk/docs/manual/doc_generic.html implies that
     *        non-static class methods can't be used with the generic call convention, but then why is there a WRAP_MFN
     *        macro? Is auxiliary supposed to be used like THISCALL_ASGLOBAL?
     * @return The result of the registration.
     * @sa Annotations.hpp
     */
    template <std::meta::info F> int RegisterGlobalFunction(void* auxiliary = nullptr);

    /**
     * Version of RegisterGlobalFunction that writes the call convention used into an out reference.
     * Primarily used for testing.
     */
    template <std::meta::info F>
    int RegisterGlobalFunction(AS_NAMESPACE_QUALIFIER asDWORD& callConvOut, void* auxiliary = nullptr);

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

    // MARK: Object Types

    // template <std::meta::info T> int RegisterObjectType();

private:
    /**
     * Points to the AngelScript engine instance this wrapper interfaces with.
     * Becomes either Owned or Shared depending on how the Engine was constructed.
     */
    std::unique_ptr<Object<AS_NAMESPACE_QUALIFIER asIScriptEngine>> m_engine;
};
} // namespace as

#include <AngelScriptWrapper/Engine.tpp>
