#pragma once
#include "Gng2D/commons/assert.hpp"

#define COMP_ARG_CASE(ARG_NAME, ASSIGN_TO)                                                         \
    case ARG_NAME##_hs:                                                                            \
        {                                                                                          \
            using ArgType = decltype(ASSIGN_TO);                                                   \
            if constexpr (std::is_same_v<ArgType, std::string>)                                    \
            {                                                                                      \
                if (arg.type().info() == entt::type_id<const char*>())                             \
                {                                                                                  \
                    void* erasedAssign          = &ASSIGN_TO;                                      \
                    *(std::string*)erasedAssign = std::string(arg.cast<const char*>());            \
                }                                                                                  \
                else                                                                               \
                {                                                                                  \
                    GNG2D_ASSERT(arg.type().info() == entt::type_id<std::string>(),                \
                                 "Actual type:", arg.type().info().name());                        \
                    const auto data = arg.cast<ArgType>();                                         \
                    ASSIGN_TO       = std::move(data);                                             \
                }                                                                                  \
            }                                                                                      \
            else                                                                                   \
            {                                                                                      \
                auto data = arg.allow_cast<ArgType>().try_cast<ArgType>();                         \
                GNG2D_ASSERT(data, #ARG_NAME "invalid type:", arg.type().info().name());           \
                ASSIGN_TO = *data;                                                                 \
            }                                                                                      \
        }                                                                                          \
        break

#define UNKNOWN_ARG_CASE                                                                           \
    [[unlikely]] default:                                                                          \
        LOG::WARN("Unknown argument:", id, "with type:", arg.type().info().name());                \
        break
