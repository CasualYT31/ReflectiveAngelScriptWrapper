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
/**
 * Options to set when registering a global property.
 */
struct GlobalPropertyOptions {
    /**
     * Set this flag to true if you want your global property to be const.
     */
    bool constant = false;
};

/**
 * A wrapper around an asIScriptEngine instance.
 */
struct Engine {
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

    /**
     * Sets the default call convention for all functions registered with the application interface.
     * The starting default is CDecl.
     *
     * Usually, Engine will be able to deduce a function's call convention via reflection. E.g. if a function is a
     * non-static member of a class, asCALL_THISCALL or some variation of it will always be used. AngelScript's generic
     * call convention is also very easy to deduce via reflection. But it is not currently possible via reflection to
     * tell when CDecl or StdCall is being used, so the decision was made to introduce two features:
     * as::CDecl/as::StdCall annotations (to explicitly mark functions as using one call convention), and this method
     * (which, for most people, eliminates the need to explicitly annotate functions at all).
     * @param callConv asCALL_CDECL or asCALL_STDCALL.
     * @return asINVALID_ARG if an invalid call convention was given, otherwise the value of callConv is returned.
     */
    int SetDefaultCallConvention(AS_NAMESPACE_QUALIFIER asDWORD callConv) noexcept;

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

    template <std::meta::info F> int RegisterGlobalFunction(void* auxiliary = nullptr);

    template <std::meta::info F>
    int RegisterGlobalFunction(AS_NAMESPACE_QUALIFIER asDWORD& callConvOut, void* auxiliary = nullptr);

    // MARK: Object Types

    // template <std::meta::info T> int RegisterObjectType();

private:
    /**
     * Combines the result of FuncCallConv() with the assigned default call convention to determine which call
     * convention should bs used.
     * @param deducedCallConv The statically-deduced call convention.
     * @return deducedCallConv if it is greather than or equal to 0, otherwise the default call convention assigned to
     *         this Engine wrapper. If the default call convention supports it, it will also handle _OBJFIRST and
     *         _OBJLAST variants, if specified in the result of the statically-duduced call convention.
     */
    AS_NAMESPACE_QUALIFIER asDWORD applyDefaultCallConventionAsFallback(const int deducedCallConv) const;

    /**
     * Points to the AngelScript engine instance this wrapper interfaces with.
     * Becomes either Owned or Shared depending on how the Engine was constructed.
     */
    std::unique_ptr<Object<AS_NAMESPACE_QUALIFIER asIScriptEngine>> m_engine;

    /**
     * The default call convention to use if a given function's calling convention can't be deduced.
     */
    AS_NAMESPACE_QUALIFIER asDWORD m_defaultCallConvention = AS_NAMESPACE_QUALIFIER asCALL_CDECL;
};
} // namespace as

#include <AngelScriptWrapper/Engine.tpp>
