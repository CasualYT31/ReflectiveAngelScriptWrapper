#pragma once

#include <aswrappedcall.h>

namespace as {
namespace detail {
inline void AddAuxiliaryObjects(AuxiliaryMap& map) {}

template <typename... Ts>
void AddAuxiliaryObjects(AuxiliaryMap& map, AuxiliaryKeyIn k, AuxiliaryPtr const v, Ts... kvs) {
    assert(!map.contains(k) && "auxiliary map already has an entry for this key");
    map[k] = v;
    AddAuxiliaryObjects(map, kvs...);
}

template <std::meta::info I> AuxiliaryPtr FindAuxiliaryObject(AuxiliaryMap const& map, const bool expected) {
    constexpr auto label = ExtractAnnotation<I, AuxiliaryLabel>();

    if (label) {
        const auto labelStr = std::string(label->to);
        if (map.contains(labelStr)) {
            assert(expected && "function was given an auxiliary label when it was not expected to");
            return map.at(labelStr);
        }
    }

    assert(!expected && "function was not given an auxiliary label when it was expected to");
    return nullptr;
}
} // namespace detail

template <EngineOptions Opts> void Engine<Opts>::AddAuxiliaryObject(AuxiliaryKeyIn k, AuxiliaryPtr const v) {
    detail::AddAuxiliaryObjects(m_auxMap, k, v);
}

template <EngineOptions Opts>
template <typename... Ts>
void Engine<Opts>::AddAuxiliaryObjects(AuxiliaryKeyIn k, AuxiliaryPtr const v, Ts... kvs) {
    detail::AddAuxiliaryObjects(m_auxMap, k, v, kvs...);
}

template <EngineOptions Opts>
template <std::meta::info V, typename T>
    requires(!IsConst<T>)
int Engine<Opts>::RegisterGlobalProperty(T* const value) {
    if (!HasEngine()) { return AS_NAMESPACE_QUALIFIER asINVALID_ARG; }
    constexpr auto vName = as::GetIdentifierOf<V>();
    std::string name = std::string(vName);
    if constexpr (IsGlobalConstQualified<V, Opts.ConstGlobalPropertiesDefault>()) {
        constexpr auto constType = detail::OverrideTypeOf<const T, V>;
        name = std::string(constType) + " " + name;
    } else {
        constexpr auto type = detail::OverrideTypeOf<T, V>;
        name = std::string(type) + " " + name;
    }
    return Ptr()->RegisterGlobalProperty(name.c_str(), value);
}

template <EngineOptions Opts>
template <std::meta::info V, typename T>
    requires(!IsConst<T>)
int Engine<Opts>::RegisterGlobalProperty(OwnedObject<T> const& value) {
    if (!HasEngine()) { return AS_NAMESPACE_QUALIFIER asINVALID_ARG; }
    constexpr auto vName = as::GetIdentifierOf<V>();
    std::string name = std::string(vName);
    if constexpr (IsGlobalConstQualified<V, Opts.ConstGlobalPropertiesDefault>()) {
        constexpr auto constType = detail::OverrideTypeOf<const T, V>;
        name = std::string(constType) + " " + name;
    } else {
        constexpr auto type = detail::OverrideTypeOf<T, V>;
        name = std::string(type) + " " + name;
    }
    return Ptr()->RegisterGlobalProperty(name.c_str(), value.Ptr());
}

template <EngineOptions Opts> template <std::meta::info F> int Engine<Opts>::RegisterGlobalFunction() {
    AS_NAMESPACE_QUALIFIER asDWORD cc;
    return RegisterGlobalFunction<F>(cc);
}

namespace detail {
struct Func {
    AS_NAMESPACE_QUALIFIER asSFuncPtr addr;

    std::string_view decl;

    AS_NAMESPACE_QUALIFIER asDWORD callConv;

    bool objFirst;

    bool objLast;

    bool auxObj;
};

template <std::meta::info F, EngineOptions Opts, bool RC = false> Func GetFuncDetails() {
    Func details;

    constexpr auto deducedCallConv = FuncCallConv<F, Opts.CallConventionDefault>();
    constexpr auto defaultFuncDecl = GetFuncDecl<F, Opts.AutoHandleDefault, RC>();

    details.decl = defaultFuncDecl;
    details.callConv = deducedCallConv.callConv;

    if constexpr (deducedCallConv.callConv == AS_NAMESPACE_QUALIFIER asCALL_GENERIC) {
        if (!deducedCallConv.decl.empty()) { details.decl = deducedCallConv.decl; }

        if constexpr (deducedCallConv.genericType == GenericCallConvType::WrapFn) {
            details.addr = ::gw::id([:F:]).TMPL f<([:F:])>();
        } else if constexpr (deducedCallConv.genericType == GenericCallConvType::WrapMFn) {
            // Use fg variant, i.e. WRAP_MFN_GLOBAL.
            // addr = ::gw::id(&[:F:]).TMPL fg<&([:F:])>();
            // Compiler doesn't like WRAP_MFN_GLOBAL... Not sure why it exists if non-static class methods can't be used
            // with the generic call convention anyway.
            details.addr = ::gw::id([:F:]).TMPL f<([:F:])>();
        } else if constexpr (deducedCallConv.genericType == GenericCallConvType::WrapObjFirst) {
            // TODO: test as part of object type registration work.
            details.addr = ::gw::id([:F:]).TMPL of<([:F:])>();
        } else if constexpr (deducedCallConv.genericType == GenericCallConvType::WrapObjLast) {
            // TODO: test as part of object type registration work.
            details.addr = ::gw::id([:F:]).TMPL ol<([:F:])>();
        } else /* GenericCallConvType::None */ {
            // Even if the function is a class member, it will always be static.
            details.addr = AS_NAMESPACE_QUALIFIER asFunctionPtr(&[:F:]);
        }

    } else {
        if constexpr (std::meta::is_class_member(F) && !std::meta::is_static_member(F)) {
            details.addr = AS_NAMESPACE_QUALIFIER asSMethodPtr<sizeof(decltype(&[:F:]))>::Convert(&[:F:]);
        } else {
            details.addr = AS_NAMESPACE_QUALIFIER asFunctionPtr(&[:F:]);
        }
    }

    details.objFirst = details.callConv == AS_NAMESPACE_QUALIFIER asCALL_CDECL_OBJFIRST
                       || details.callConv == AS_NAMESPACE_QUALIFIER asCALL_THISCALL_OBJFIRST;
    details.objLast = details.callConv == AS_NAMESPACE_QUALIFIER asCALL_CDECL_OBJLAST
                      || details.callConv == AS_NAMESPACE_QUALIFIER asCALL_THISCALL_OBJLAST;
    details.auxObj = details.objFirst || details.objLast;

    return details;
}
} // namespace detail

template <EngineOptions Opts>
template <std::meta::info F>
int Engine<Opts>::RegisterGlobalFunction(AS_NAMESPACE_QUALIFIER asDWORD& callConvOut) {
    if (!HasEngine()) { return AS_NAMESPACE_QUALIFIER asINVALID_ARG; }

    auto funcDetails = detail::GetFuncDetails<F, Opts, true>();

    bool expectAux = false;
    if (funcDetails.callConv == AS_NAMESPACE_QUALIFIER asCALL_THISCALL
        || funcDetails.callConv == AS_NAMESPACE_QUALIFIER asCALL_THISCALL_OBJFIRST
        || funcDetails.callConv == AS_NAMESPACE_QUALIFIER asCALL_THISCALL_OBJLAST) {
        funcDetails.callConv = AS_NAMESPACE_QUALIFIER asCALL_THISCALL_ASGLOBAL;
        expectAux = true;
    }

    const auto auxiliary = detail::FindAuxiliaryObject<F>(m_auxMap, expectAux);

    return Ptr()->RegisterGlobalFunction(
        funcDetails.decl.data(), funcDetails.addr, callConvOut = funcDetails.callConv, auxiliary
    );
}

template <EngineOptions Opts> template <std::meta::info F> int Engine<Opts>::RegisterFuncdef() {
    if (!HasEngine()) { return AS_NAMESPACE_QUALIFIER asINVALID_ARG; }
    constexpr auto decl = GetFuncDecl<F, Opts.AutoHandleDefault, true>();
    return Ptr()->RegisterFuncdef(decl.data());
}

template <EngineOptions Opts> template <std::meta::info T> int Engine<Opts>::RegisterTypedef() {
    if (!HasEngine()) { return AS_NAMESPACE_QUALIFIER asINVALID_ARG; }
    constexpr auto typedefName = GetIdentifierOf<T>();
    // Since AngelScript only supports primitive types here, we don't need to worry about SubTypeLists or other complex
    // annotations or types.
    using DealiasedT = typename[:std::meta::dealias(T):];
    constexpr auto typedefDecl = TypeName<DealiasedT>;
    return Ptr()->RegisterTypedef(typedefName.data(), typedefDecl.data());
}

template <EngineOptions Opts> template <std::meta::info E> int Engine<Opts>::RegisterEnum() {
    if (!HasEngine()) { return AS_NAMESPACE_QUALIFIER asINVALID_ARG; }
    constexpr auto enumName = GetIdentifierOf<E>();
    static constexpr auto values = std::define_static_array(std::meta::enumerators_of(E));

    const auto typeId = Ptr()->RegisterEnum(enumName.data());

    template for (constexpr auto v : values) {
        const auto r = Ptr()->RegisterEnumValue(enumName.data(), GetIdentifierOf<v>().data(), static_cast<int>([:v:]));
        if (r < 0) { return r; }
    }

    return typeId;
}

template <EngineOptions Opts> template <std::meta::info I, bool R> int Engine<Opts>::RegisterInterface() {
    if (!HasEngine()) { return AS_NAMESPACE_QUALIFIER asINVALID_ARG; }
    int r = 0;
    static constexpr auto interfaceHierarchy = GetClassHierarchy<I>(R);

    template for (constexpr auto i : interfaceHierarchy) {
        using Type = [:i.type:];
        if (HasRegisteredInterface<Type>()) { continue; }

        constexpr auto interfaceName = TypeName<Type>;
        if (r = Ptr()->RegisterInterface(interfaceName.data()); r < 0) { return r; }
        m_interfaces.insert(std::type_index(typeid(Type)));

        static constexpr auto members = i.members;
        template for (constexpr auto m : members) {
            if constexpr (std::meta::is_pure_virtual(m.member) && !std::meta::is_operator_function(m.member)) {
                constexpr auto decl = GetFuncDecl<m.member, Opts.AutoHandleDefault>();
                if (r = Ptr()->RegisterInterfaceMethod(interfaceName.data(), decl.data()); r < 0) { return r; }
            }
        }
    }

    return r;
}

/* Reference types TODO list:
[x] 1. Factory function, default and custom.
[x] 2. Factory functions with auxiliary objects.
[ ] 3. List factory functions (see point 3 under value types).
[x] 4. AddRef and Release behaviours, including asOBJ_NOCOUNT.
[ ] 5. Support scoped reference types that only have Release behaviour. ScopedReferenceType.hpp.
    a. https://www.angelcode.com/angelscript/sdk/docs/manual/doc_adv_scoped_type.html
       These types have their own weird @ and &in/&out behaviour... Might need to make this a special type like
       as::RefType.
[x] 6. Support asOBJ_NOHANDLE. No factory, addref or release behaviours.
[x] 7. Support inheritance. Automatically add opCast and opImplCast methods. Make it work across multiple levels of the
   hierarchy.
[x] 8. Support weak refs.
*/

/* Value types TODO list:
[x] 1. Tell the difference between a POD type and a non-POD type.
[ ] 2. Figure out how constructors and destructors will work (similarly to factory functions probably).
[ ] 3. Figure out how list constructors will work (want to minimize manually writing out the pattern as much as
       possible).
[x] 4. Use asGetTypeTraits().
[x] 5. asOBJ_APP_CLASS_MORE_CONSTRUCTORS.
[x] 6. asOBJ_APP_CLASS_ALLINTS.
[x] 7. asOBJ_APP_CLASS_ALLFLOATS.
[x] 8. asOBJ_APP_CLASS_ALIGN8.
[x] 9. asOBJ_APP_CLASS_UNION.
*/

/* Operators TODO list:
[ ] 1. Create a mapping from display_string_of() to AngelScript op method names.
       This might not be required: I found
https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2025/p2996r12.html#meta.reflection.operators-operator-representations
       which seems to do something like this for me.
[ ] 2. Support both static and non-static operators so long as they are within the class's scope.
[ ] 3. Do we want to ignore special member functions?
*/

/* Methods TODO list:
[x] 1. Ignore non-public methods, along with constructors and destructors.
[ ] 2. Do we support composite members? Inclination is to ignore this for now, but it might be useful.
[x] 3. Detect property accessors, i.e. get_XYZ and set_XYZ methods, and append "property" to the declaration.
*/

/* Properties TODO list:
[x] 1. Public properties only.
[x] 2. Supporting pointer properties. They require special & syntax, regardless of ref or value type?
[ ] 3. Ignore composite members for now.
*/

/* Garbage-collected types TODO list:
[ ] 1. Implement GarbageCollectedReferenceType.hpp ...
[ ] 2. ... and GarbageCollectedValueType.hpp abstract classes.
[ ] 3. Detect presence of garbage collection-related methods/functions and apply asOBJ_GC flag, etc.
*/

/* Template types TODO list:
[ ] 1. Factory (ref) and constructor (value) hidden parameter: map asITypeInfo* to int&in.
[ ] 2. Support 1 for list factories/constructors, too.
[ ] 3. if_handle_then_const.
[ ] 4. asBEHAVE_TEMPLATE_CALLBACK behaviour.
[ ] 5. Template specializations.
*/

/* Scoped funcdefs TODO list:
[ ] 1. Support scoped funcdefs, i.e. if the given function declaration is within a class, scope it to that class:
   https://www.angelcode.com/angelscript/sdk/docs/manual/classas_i_script_engine.html#a03c1a2cc23ae4b742c927f3472a1a4f7.
[ ] 2. Support scoped funcdefs for template types.
*/

namespace detail {
using EnginePtr = AS_NAMESPACE_QUALIFIER asIScriptEngine* const;

struct OrganizedClassInformation : ClassInformation {
    const StructuralSpan<const ClassMember> factoryBehaviours;

    const StructuralSpan<const ClassMember> addRefBehaviours;

    const StructuralSpan<const ClassMember> releaseBehaviours;

    const StructuralSpan<const ClassMember> getWeakrefFlagBehaviours;

    const StructuralSpan<const ClassMember> constructorBehaviours;

    const StructuralSpan<const ClassMember> destructorBehaviours;

    const StructuralSpan<const ClassMember> methods;

    const StructuralSpan<const ClassMember> properties;
};

template <OrganizedClassInformation T>
int RegisterObjectType(EnginePtr engine, std::string_view const& typeName, const bool isPodType) {
    using type = [:T.type:];
    constexpr auto typeIsRef = IsRefType<type>();
    constexpr auto typeSize = typeIsRef ? 0 : sizeof(type);

    AS_NAMESPACE_QUALIFIER asQWORD flags = typeIsRef
                                               ? AS_NAMESPACE_QUALIFIER asOBJ_REF
                                               : AS_NAMESPACE_QUALIFIER asOBJ_VALUE | GetValueTypeTraits<T.type>();

    if constexpr (typeIsRef && T.addRefBehaviours.empty() && T.releaseBehaviours.empty()) {
        if constexpr (T.factoryBehaviours.empty()) {
            flags |= AS_NAMESPACE_QUALIFIER asOBJ_NOHANDLE;
        } else {
            flags |= AS_NAMESPACE_QUALIFIER asOBJ_NOCOUNT;
        }
    }
    if (isPodType) { flags |= AS_NAMESPACE_QUALIFIER asOBJ_POD; }

    return engine->RegisterObjectType(
        typeName.data(), typeSize, static_cast<AS_NAMESPACE_QUALIFIER asEObjTypeFlags>(flags)
    );
}

template <std::meta::info F, EngineOptions Opts>
int RegisterObjectBehaviour(
    EnginePtr engine, std::string_view const& typeName, const AS_NAMESPACE_QUALIFIER asEBehaviours behaviourCode
) {
    const auto funcDetails = detail::GetFuncDetails<F, Opts, true>();

    return engine->RegisterObjectBehaviour(
        typeName.data(), behaviourCode, funcDetails.decl.data(), funcDetails.addr, funcDetails.callConv
    );
}

template <std::meta::info F, EngineOptions Opts>
int RegisterObjectBehaviour(
    EnginePtr engine,
    std::string_view const& typeName,
    const AS_NAMESPACE_QUALIFIER asEBehaviours behaviourCode,
    AuxiliaryMap const& auxMap
) {
    const auto funcDetails = detail::GetFuncDetails<F, Opts, true>();

    void* const aux = detail::FindAuxiliaryObject<F>(auxMap, funcDetails.auxObj);

    return engine->RegisterObjectBehaviour(
        typeName.data(), behaviourCode, funcDetails.decl.data(), funcDetails.addr, funcDetails.callConv, aux
    );
}

template <std::meta::info F, EngineOptions Opts, typename T>
int RegisterObjectFactoryFunction(EnginePtr engine, std::string_view const& typeName, AuxiliaryMap const& aux) {
    constexpr auto ds = std::meta::display_string_of(F);
    static_assert(
        std::meta::is_static_member(F), std::string(ds) + " was marked as a factory function, but it is non-static"
    );
    static_assert(
        std::meta::return_type_of(F) == ^^T*,
        std::string(ds) + " was marked as a factory function, but it does not return a pointer to "
            + std::string(typeName)
    );

    return RegisterObjectBehaviour<F, Opts>(engine, typeName, AS_NAMESPACE_QUALIFIER asBEHAVE_FACTORY, aux);
}

template <std::meta::info F, EngineOptions Opts, typename T>
int RegisterObjectConstructorFunction(EnginePtr engine, std::string_view const& typeName, AuxiliaryMap const&) {
    constexpr auto ds = std::meta::display_string_of(F);
    static_assert(
        std::meta::is_static_member(F), std::string(ds) + " was marked as a constructor function, but it is non-static"
    );

    return RegisterObjectBehaviour<F, Opts>(engine, typeName, AS_NAMESPACE_QUALIFIER asBEHAVE_CONSTRUCT);
}

template <std::meta::info F, EngineOptions Opts, typename T>
int RegisterObjectDestructorFunction(EnginePtr engine, std::string_view const& typeName, AuxiliaryMap const&) {
    constexpr auto ds = std::meta::display_string_of(F);
    static_assert(
        std::meta::is_static_member(F), std::string(ds) + " was marked as a destructor function, but it is non-static"
    );

    return RegisterObjectBehaviour<F, Opts>(engine, typeName, AS_NAMESPACE_QUALIFIER asBEHAVE_DESTRUCT);
}

template <std::meta::info F, EngineOptions Opts, typename T>
int RegisterObjectAddRefFunction(EnginePtr engine, std::string_view const& typeName, AuxiliaryMap const& aux) {
    return RegisterObjectBehaviour<F, Opts>(engine, typeName, AS_NAMESPACE_QUALIFIER asBEHAVE_ADDREF, aux);
}

template <std::meta::info F, EngineOptions Opts, typename T>
int RegisterObjectReleaseFunction(EnginePtr engine, std::string_view const& typeName, AuxiliaryMap const& aux) {
    return RegisterObjectBehaviour<F, Opts>(engine, typeName, AS_NAMESPACE_QUALIFIER asBEHAVE_RELEASE, aux);
}

template <std::meta::info F, EngineOptions Opts, typename T>
int RegisterObjectGetWeakrefFlagFunction(EnginePtr engine, std::string_view const& typeName, AuxiliaryMap const& aux) {
    return RegisterObjectBehaviour<F, Opts>(engine, typeName, AS_NAMESPACE_QUALIFIER asBEHAVE_GET_WEAKREF_FLAG, aux);
}

template <std::meta::info F, EngineOptions Opts, typename T>
int RegisterObjectMethod(EnginePtr engine, std::string_view const& typeName, AuxiliaryMap const& aux) {
    const auto funcDetails = detail::GetFuncDetails<F, Opts>();

    if (funcDetails.callConv == AS_NAMESPACE_QUALIFIER asCALL_CDECL
        || funcDetails.callConv == AS_NAMESPACE_QUALIFIER asCALL_STDCALL) {
        // A static method was given that didn't explicitly request OBJFIRST or OBJLAST,
        // do not register it.
        return 0;
    }

    return engine->RegisterObjectMethod(
        typeName.data(), funcDetails.decl.data(), funcDetails.addr, funcDetails.callConv
    );
}

template <std::meta::info P, EngineOptions Opts, typename T>
int RegisterObjectProperty(EnginePtr engine, std::string_view const& typeName, AuxiliaryMap const& aux) {
    using PType = [:std::meta::type_of(P):];
    constexpr auto identifierOf = std::meta::identifier_of(P);

    std::string decl;
    if constexpr (IsPointer<PType>) {
        constexpr auto typeOf = detail::OverrideTypeOf<std::remove_pointer_t<PType>, P>;
        decl = std::string(typeOf) + " &" + std::string(identifierOf);
    } else {
        constexpr auto typeOf = TypeOf<P>;
        decl = std::string(typeOf) + " " + std::string(identifierOf);
    }

    return engine->RegisterObjectProperty(typeName.data(), decl.c_str(), std::meta::offset_of(P).bytes);
}

/* Operator overloading notes

opNeg
T operator-()

opCom
T operator~()

opPreInc
T& operator++()

opPreDec
T& operator--()

opPostInc
T operator++(int)

opPostDev
T operator--(int)

opEquals
bool operator==(any)
The is operator should be used if a handle is given.

opCmp
bool operator<(any)
bool operator<=(any)
bool operator>(any)
bool operator>=(any)
bool operator<=>(any)
We will need to generate some kind of wrapper for this?
T > any? +ive
T == any? 0
T < any? -ive

opAssign
T& operator=(any)

opAddAssign
T& operator+=(any)

opSubAssign
T& operator-=(any)

opMulAssign
T& operator*=(any)

opDivAssign
T& operator/=(any)

opModAssign
T& operator%=(any)

opPowAssign
Has no C++ equivalent...
Will need to write it as T& operatorPowAssign(any)?

opAndAssign
T& operator&=(any)

opOrAssign
T& operator|=(any)

opXorAssign
T& operator^=(any)

opShlAssign
T& operator<<=(any)

opShrAssign
T& operator>>(any)

opUShrAssign
T& operator>>(unsigned)

opAdd
T operator+(any)

opAdd_r
static T operator+[[=as::ObjLast]](any, T const&)
OR
static T operator+[[=as::ObjFirst]](T const&, any)

opSub
T operator-(any)

opMul
T operator*(any)

opDiv
T operator/(any)

opMod
T operator%(any)

opPow
Has no C++ equivalent...
Will need to write it as T operatorPow(any)?

opAnd
T operator&(any)

opOr
T operator|(any)

opXor
T operator^(any)

opShl
T operator<<(any)

opShr
T operator>>(any)

opUShr
T operator>>(unsigned)

opIndex
any operator[](...)

opCall
any operator()(...)

opConv
explicit operator U()

opImplConv
operator U()

opForBegin
Has no standard C++ equivalent...
Will need to write it as int/T* operatorForBegin()?

opForEnd
Has no standard C++ equivalent...
Will need to write it as bool operatorForEnd(int/T*)?

opForNext
Has no standard C++ equivalent...
Will need to write it as int/T* operatorForNext(int/T*)?

opForValueX
Has no standard C++ equivalent...
Will need to write it as any operatorForValue[X](int/T*)?

*/

template <ClassInformation I> OrganizedClassInformation consteval OrganizedClassHierarchy() {
    std::vector<ClassMember> factoryBehaviours;
    std::vector<ClassMember> addRefBehaviours;
    std::vector<ClassMember> releaseBehaviours;
    std::vector<ClassMember> getWeakrefFlagBehaviours;
    std::vector<ClassMember> constructorBehaviours;
    std::vector<ClassMember> destructorBehaviours;
    std::vector<ClassMember> methods;
    std::vector<ClassMember> properties;

    static constexpr auto members = I.members;
    template for (constexpr auto m : members) {
        if constexpr (std::meta::is_function(m.member)) {
            if constexpr (!m.inherited) {
                // Some behaviours cannot be inherited, check for these first and ignore them if they are found in
                // base classes.
                if constexpr (IsBehaviour<m.member, AS_NAMESPACE_QUALIFIER asBEHAVE_FACTORY>()) {
                    factoryBehaviours.push_back(m);
                    continue;
                } else if constexpr (IsBehaviour<m.member, AS_NAMESPACE_QUALIFIER asBEHAVE_CONSTRUCT>()) {
                    constructorBehaviours.push_back(m);
                    continue;
                } else if constexpr (IsBehaviour<m.member, AS_NAMESPACE_QUALIFIER asBEHAVE_DESTRUCT>()) {
                    destructorBehaviours.push_back(m);
                    continue;
                }
            }
            if constexpr (IsBehaviour<m.member, AS_NAMESPACE_QUALIFIER asBEHAVE_ADDREF>()) {
                addRefBehaviours.push_back(m);
            } else if constexpr (IsBehaviour<m.member, AS_NAMESPACE_QUALIFIER asBEHAVE_RELEASE>()) {
                releaseBehaviours.push_back(m);
            } else if constexpr (IsBehaviour<m.member, AS_NAMESPACE_QUALIFIER asBEHAVE_GET_WEAKREF_FLAG>()) {
                getWeakrefFlagBehaviours.push_back(m);
            } else if constexpr (std::meta::is_operator_function(m.member)) {
                // static_assert(false, std::meta::display_string_of(m.member));
            } else {
                methods.push_back(m);
            }
        } else if constexpr (std::meta::is_nonstatic_data_member(m.member)) {
            properties.push_back(m);
        } // else ignore member.
    }

    return OrganizedClassInformation{
        ClassInformation{ I },
        std::define_static_array(factoryBehaviours),
        std::define_static_array(addRefBehaviours),
        std::define_static_array(releaseBehaviours),
        std::define_static_array(getWeakrefFlagBehaviours),
        std::define_static_array(constructorBehaviours),
        std::define_static_array(destructorBehaviours),
        std::define_static_array(methods),
        std::define_static_array(properties),
    };
}

// Adapted from https://www.angelcode.com/angelscript/sdk/docs/manual/doc_adv_class_hierarchy.html.
template <class From, class To> To* RefCast(From* from) {
    // If the handle already is a null handle, then just return the null handle.
    // Otherwise, try to dynamically cast the pointer to the wanted type.
    // The operator will be registered with the auto handle, so the AddRef will be handled for us.
    return dynamic_cast<To*>(from);
}
} // namespace detail

#define AS_DETAIL_REGISTER_MEMBERS(span, func)                                                                         \
    {                                                                                                                  \
        static constexpr auto members = organizedC.span;                                                               \
        template for (constexpr auto m : members) {                                                                    \
            if (const auto r = func<m.member, Opts, type>(Ptr(), className, m_auxMap); r < 0) { return r; }            \
        }                                                                                                              \
    }

template <EngineOptions Opts> template <std::meta::info T, bool R> int Engine<Opts>::RegisterObjectType() {
    if (!HasEngine()) { return AS_NAMESPACE_QUALIFIER asINVALID_ARG; }
    static constexpr auto classHierarchy = GetClassHierarchy<T>(R);
    int typeId = -1;

    template for (constexpr auto c : classHierarchy) {
        using type = [:c.type:];
        if (HasRegisteredObjectType<type>()) { continue; }

        // 1. Organize the class hierarchy so we know ahead of time which behaviors the type will have.
        constexpr auto className = TypeName<type>;
        static constexpr auto organizedC = detail::OrganizedClassHierarchy<c>();
        const auto isPod = as::IsAngelScriptPodType<c.type>(m_podTypes);

        // 2. Register the type.
        if (const auto r = detail::RegisterObjectType<organizedC>(Ptr(), className, isPod); r < 0) {
            return r;
        } else if (typeId < 0) {
            typeId = r;
        }
        m_types.insert(std::type_index(typeid(type)));
        if (isPod) { AddPodType<type>(); }

        // 3. Register properties, members and behaviours.
        AS_DETAIL_REGISTER_MEMBERS(addRefBehaviours, detail::RegisterObjectAddRefFunction);
        AS_DETAIL_REGISTER_MEMBERS(releaseBehaviours, detail::RegisterObjectReleaseFunction);
        AS_DETAIL_REGISTER_MEMBERS(getWeakrefFlagBehaviours, detail::RegisterObjectGetWeakrefFlagFunction);
        // It is important to register AddRef and Release behaviours before Factory behaviours.
        // E.g. you may wish to register a copy factory that accepts a handle to your ref type.
        AS_DETAIL_REGISTER_MEMBERS(factoryBehaviours, detail::RegisterObjectFactoryFunction);
        AS_DETAIL_REGISTER_MEMBERS(constructorBehaviours, detail::RegisterObjectConstructorFunction);
        AS_DETAIL_REGISTER_MEMBERS(destructorBehaviours, detail::RegisterObjectDestructorFunction);
        AS_DETAIL_REGISTER_MEMBERS(methods, detail::RegisterObjectMethod);
        AS_DETAIL_REGISTER_MEMBERS(properties, detail::RegisterObjectProperty);
    }

    // Register class relationships separately as all base and derived types have been registered by now.
    // We still carry out this process for types that have already been registered. But don't bother with
    // this process at all if recursion is turned off, as we won't necessarily have the base types to cast to.
    if constexpr (R) {
        template for (constexpr auto c : classHierarchy) {
            using type = [:c.type:];
            constexpr auto className = TypeName<type>;
            const auto classNameStr = std::string(className);

            static constexpr auto bases = c.bases;
            template for (constexpr auto b : bases) {
                using baseType = [:b:];
                constexpr auto baseClassName = TypeName<baseType>;

                const auto implCastDecl = std::string(baseClassName) + "@+ opImplCast()";

                if (m_castingOperators[classNameStr].contains(implCastDecl)) {
                    continue;
                } else {
                    m_castingOperators[classNameStr].insert(implCastDecl);
                }

                const auto constImplCastDecl = "const " + implCastDecl + " const";

                if (const auto r = Ptr()->RegisterObjectMethod(
                        className.data(),
                        implCastDecl.c_str(),
                        AS_NAMESPACE_QUALIFIER asFUNCTION((detail::RefCast<type, baseType>)),
                        AS_NAMESPACE_QUALIFIER asCALL_CDECL_OBJLAST
                    );
                    r < 0) {
                    return r;
                }
                if (const auto r = Ptr()->RegisterObjectMethod(
                        className.data(),
                        constImplCastDecl.c_str(),
                        AS_NAMESPACE_QUALIFIER asFUNCTION((detail::RefCast<type, baseType>)),
                        AS_NAMESPACE_QUALIFIER asCALL_CDECL_OBJLAST
                    );
                    r < 0) {
                    return r;
                }
            }

            static constexpr auto derivatives = c.derivatives;
            template for (constexpr auto d : derivatives) {
                using derivedType = [:d:];
                constexpr auto derivedClassName = TypeName<derivedType>;

                const auto castDecl = std::string(derivedClassName) + "@+ opCast()";

                if (m_castingOperators[classNameStr].contains(castDecl)) {
                    continue;
                } else {
                    m_castingOperators[classNameStr].insert(castDecl);
                }

                const auto constCastDecl = "const " + castDecl + " const";

                if (const auto r = Ptr()->RegisterObjectMethod(
                        className.data(),
                        castDecl.c_str(),
                        AS_NAMESPACE_QUALIFIER asFUNCTION((detail::RefCast<type, derivedType>)),
                        AS_NAMESPACE_QUALIFIER asCALL_CDECL_OBJLAST
                    );
                    r < 0) {
                    return r;
                }
                if (const auto r = Ptr()->RegisterObjectMethod(
                        className.data(),
                        constCastDecl.c_str(),
                        AS_NAMESPACE_QUALIFIER asFUNCTION((detail::RefCast<type, derivedType>)),
                        AS_NAMESPACE_QUALIFIER asCALL_CDECL_OBJLAST
                    );
                    r < 0) {
                    return r;
                }
            }
        }
    }

    return typeId;
}

#undef AS_DETAIL_REGISTER_MEMBERS
} // namespace as
