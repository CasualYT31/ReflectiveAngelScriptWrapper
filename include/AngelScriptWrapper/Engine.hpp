/**
 * @file Engine.hpp
 * Defines the wrapper class for an asIScriptEngine instance.
 */

#pragma once

#include <angelscript.h>
#include <AngelScriptWrapper/Object.hpp>
#include <memory>
#include <meta>

namespace as {
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
        return &**m_engine;
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
     * @tparam T The type of global property to register. AngelScript does not allow registering global properties
     *         linked to const C++ objects. If you want to register a constant global property, set the constant
     *         parameter to true.
     * @param name The name to give to the global property.
     * @param value Pointer to the value to give to the global property. You are expected to manage the global
     *        property's value as you would without this wrapper.
     * @param constant True if you want to register the property as constant.
     * @return The result of the registration.
     */
    template <typename T>
        requires(!IsConst<T>)
    int RegisterGlobalProperty(std::string name, T* const value, const bool constant = false);

    /**
     * Registers a global property of a template AngelScript type.
     * @tparam Alias The std::meta::info object of the type alias defined by your instantiation of Specialize.
     * @tparam T The type of global property to register. AngelScript does not allow registering global properties
     *         linked to const C++ objects. If you want to register a constant global property, use a Specialization of
     *         a const base type, but make sure that T is the non-const qualified version of the base type.
     * @param name The name to give to the global property.
     * @param value Pointer to the value to give to the global property. You are expected to manage the global
     *        property's value as you would without this wrapper.
     * @return The result of the registration.
     */
    template <std::meta::info Alias, typename T>
        requires(!IsConst<T>)
    int RegisterGlobalProperty(std::string name, T* const value);

private:
    /**
     * Points to the AngelScript engine instance this wrapper interfaces with.
     * Becomes either Owned or Shared depending on how the Engine was constructed.
     */
    std::unique_ptr<Object<AS_NAMESPACE_QUALIFIER asIScriptEngine>> m_engine;
};
} // namespace as

#include <AngelScriptWrapper/Engine.tpp>
