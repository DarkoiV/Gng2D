#pragma once
#include "Gng2D/commons/types.hpp" // IWYU pragma: keep

#define component_property_name(ARG)                                                               \
    static constexpr char const* NAME = #ARG;                                                      \
    static constexpr StringHash  ID   = entt::hashed_string::value(#ARG)

#define component_property_is_detail                                                               \
    static constexpr std::monostate IS_DETAIL { }
