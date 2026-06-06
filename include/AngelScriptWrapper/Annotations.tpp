#pragma once

BEGIN_AS_NAMESPACE

class asIScriptFunction;
class CDateTime;
class Complex;
class CScriptAny;
class CScriptArray;
class CScriptDictionary;
class CScriptDictValue;
class CScriptFile;
class CScriptFileSystem;
class CScriptGrid;
class CScriptHandle;
class CScriptSocket;
class CScriptWeakRef;

END_AS_NAMESPACE

namespace as {
// MARK: Helpers

template <std::meta::info I, typename T> consteval std::span<const std::meta::info> GetAnnotations() {
    return std::define_static_array(std::meta::annotations_of_with_type(I, ^^T));
}

template <std::meta::info I, typename T> constexpr std::optional<T> ExtractAnnotation() {
    constexpr auto annotations = GetAnnotations<I, ^^T>();
    static_assert(
        annotations.size() <= 1,
        std::string(std::meta::display_string_of(I)) + " was given more than one "
            + std::string(std::meta::display_string_of(^^T)) + " annotation"
    );
    if constexpr (annotations.empty()) {
        return std::nullopt;
    } else {
        return std::meta::extract<T>(annotations[0]);
    }
}

template <std::meta::info I, typename T> constexpr bool HasAnnotation() {
    return !GetAnnotations<I, T>().empty();
}

template <bool... Bs> consteval bool AtLeastOneOf() {
    bool foundOneTrue = false;
    template for (auto b : { Bs... }) {
        if (b) {
            if (foundOneTrue) {
                return false;
            } else {
                foundOneTrue = true;
            }
        }
    }
    return true;
}

// MARK: Const

template <std::meta::info O, bool Fallback> constexpr bool IsGlobalConstQualified() {
    constexpr auto HasConst = HasAnnotation<O, decltype(GlobalConst)>();
    constexpr auto HasNotConst = HasAnnotation<O, decltype(GlobalNotConst)>();
    static_assert(AtLeastOneOf<HasConst, HasNotConst>(), "detected mix of Const and NotConst annotations");
    if constexpr (HasConst) {
        return true;
    } else if constexpr (HasNotConst) {
        return false;
    } else {
        return Fallback;
    }
}

// MARK: SubTypes

namespace detail {
consteval SubTypes SubTypeList(std::vector<std::meta::info> types) {
    std::vector<std::meta::info> subTypes;
    std::vector<SubTypes> recursiveSub;
    for (auto T : types) {
        if (std::meta::has_template_arguments(T) && std::meta::template_of(T) == ^^Tmpl) {
            auto args = std::meta::template_arguments_of(T);
            subTypes.push_back(args[0]);
            args.erase(args.begin());
            recursiveSub.push_back(SubTypeList(args));
        } else {
            subTypes.push_back(T);
            recursiveSub.emplace_back();
        }
    }
    return SubTypes{
        .subTypes = std::define_static_array(subTypes),
        .recursiveSub = std::define_static_array(recursiveSub),
    };
}
} // namespace detail

template <typename... Ts> consteval SubTypes SubTypeList() {
    return detail::SubTypeList({ ^^Ts... });
}

// MARK: RefType

template <typename T> constexpr bool IsRefType() {
    using BaseT = std::remove_cvref_t<remove_all_pointers_t<T>>;
    // Unfortunately, we can't attach the RefType annotation to these classes reliably, as type attributes are ignored
    // after types are already defined, and we shouldn't force the developer to include header files in certain orders
    // to get the annotations to play nicely. So we are forced to add special cases for these here:
    if constexpr (std::is_same_v<BaseT, AS_NAMESPACE_QUALIFIER asIScriptFunction>
                  || std::is_same_v<BaseT, AS_NAMESPACE_QUALIFIER CScriptAny>
                  || std::is_same_v<BaseT, AS_NAMESPACE_QUALIFIER CScriptArray>
                  || std::is_same_v<BaseT, AS_NAMESPACE_QUALIFIER CScriptDictionary>
                  || std::is_same_v<BaseT, AS_NAMESPACE_QUALIFIER CScriptFile>
                  || std::is_same_v<BaseT, AS_NAMESPACE_QUALIFIER CScriptFileSystem>
                  || std::is_same_v<BaseT, AS_NAMESPACE_QUALIFIER CScriptGrid>
                  || std::is_same_v<BaseT, AS_NAMESPACE_QUALIFIER CScriptSocket>) {
        return true;
    } else {
        return HasAnnotation<std::meta::dealias(^^BaseT), decltype(RefType)>();
    }
}
} // namespace as
