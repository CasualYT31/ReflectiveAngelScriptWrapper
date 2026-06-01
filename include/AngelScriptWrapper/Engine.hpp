/**
 * @file Engine.hpp
 * Defines the wrapper class for an asIScriptEngine instance.
 */

#pragma once

#include <angelscript.h>
#include <AngelScriptWrapper/Object.hpp>
#include <AngelScriptWrapper/OwnedObject.hpp>
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
    Engine();

    /**
     * Initializes an engine wrapper with an existing asIScriptEngine instance.
     * @warning Usually this wrapper object will release the engine when the wrapper object is destructed, but if the
     *          wrapper is constructed in this way, then the engine will not be released and it is the responsibility of
     *          the caller to release the engine that it provides here.
     */
    Engine(AS_NAMESPACE_QUALIFIER asIScriptEngine* const engine);

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

    // template <std::meta::info F> int RegisterGlobalFunction(void* auxiliary = nullptr);

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
