#pragma once
#include "Gng2D/commons/log.hpp"

#define GNG2D_ALWAYS_ASSERT(condition, ...)                                                        \
    if (not(condition))                                                                            \
    {                                                                                              \
        LOG::FATAL("[ ASSERTION", #condition, "FAILED ]", __VA_ARGS__);                            \
    }

#ifdef NDEBUG
#define GNG2D_ASSERT(condition, ...) ((void)0)
#else
#define GNG2D_ASSERT(condition, ...) GNG2D_ALWAYS_ASSERT(condition, __VA_ARGS__)
#endif

#define GNG2D_ASSERT_CONSTEXPR(msg)                                                                \
    []<bool flag = false>()                                                                        \
    {                                                                                              \
        static_assert(flag, "Type not supported");                                                 \
    }                                                                                              \
    ()

