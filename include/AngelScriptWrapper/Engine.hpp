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
 * Options to set when registering a global property.
 */
struct GlobalPropertyOptions {
    // TODO: this should be an annotation instead of a runtime flag.
    /**
     * Set this flag to true if you want your global property to be const.
     */
    bool constant = false;
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
     *         linked to const C++ objects. If you want to register a constant global property, set the constant
     *         option to true.
     * @param value Pointer to the value to give to the global property. You are expected to manage the global
     *        property's value as you would without this wrapper.
     * @param opts The options to set for this global property.
     * @return The result of the registration.
     */
    template <std::meta::info V, typename T>
        requires(!IsConst<T>)
    int RegisterGlobalProperty(T* const value, GlobalPropertyOptions const& opts = {});

    /**
     * Registers a global property for use in the scripts whose pointer is being managed by an OwnedObject wrapper.
     * The name of the global property in the application interface is derived from the identifier of the given
     * OwnedObject instance. If you want to change this name, you will need to attach a Rename annotation to the
     * OwnedObject instance via the use of the Name() function.
     * @tparam V The std::meta::info object of your OwnedObject wrapper. This means that if your underlying object is of
     *         a specialized template type, you must attach SubTypes annotations to the Object wrapper!
     * @tparam T The type of global property to register. AngelScript does not allow registering global properties
     *         linked to const C++ objects. If you want to register a constant global property, set the constant
     *         parameter to true.
     * @param value The OwnedObject wrapper that manages the AngelScript object to use as the value of the global
     *        property. The underlying pointer is used so make sure this OwnedObject survives for as long as the script
     *        engine is running.
     * @param opts The options to set for this global property.
     * @return The result of the registration.
     */
    template <std::meta::info V, typename T>
        requires(!IsConst<T>)
    int RegisterGlobalProperty(OwnedObject<T> const& value, GlobalPropertyOptions const& opts = {});

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
