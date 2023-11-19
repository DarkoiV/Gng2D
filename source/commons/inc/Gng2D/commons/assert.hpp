#pragma once
#include "Gng2D/commons/log.hpp" // IWYU pragma: export

#define GNG2D_ALWAYS_ASSERT(condition, ...)                                                        \
    do                                                                                             \
    {                                                                                              \
        if (not(condition))                                                                        \
        {                                                                                          \
            ::Gng2D::LOG::FATAL("[ Assertion", __FILE__ ":", __LINE__, "(" #condition ")",         \
                                "failed]\n" __VA_OPT__(, ) __VA_ARGS__);                           \
        }                                                                                          \
    } while (0)

#ifdef NDEBUG
#define GNG2D_ASSERT(condition, ...) ((void)0)
#else
#define GNG2D_ASSERT(condition, ...) GNG2D_ALWAYS_ASSERT(condition, __VA_ARGS__)
#endif

#define GNG2D_ASSERT_CONSTEXPR(msg)                                                                \
    do                                                                                             \
    {                                                                                              \
        []<bool flag = false>()                                                                    \
        {                                                                                          \
            static_assert(flag, "Type not supported");                                             \
        }                                                                                          \
        ();                                                                                        \
    } while (0)
