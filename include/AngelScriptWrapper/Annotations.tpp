#pragma once

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
    return HasAnnotation<std::meta::dealias(^^std::remove_cvref_t<remove_all_pointers_t<T>>), decltype(RefType)>();
}
} // namespace as
