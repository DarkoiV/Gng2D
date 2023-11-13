#pragma once
#include "Gng2D/commons/assert.hpp"

#define COMP_ARG_CASE(ARG_NAME, ARG_TYPE, ASSIGN_TO)                                               \
    case ARG_NAME##_hs:                                                                            \
        {                                                                                          \
            auto data = arg.allow_cast<ARG_TYPE>().try_cast<ARG_TYPE>();                           \
            GNG2D_ASSERT(data, #ARG_NAME "has to be" #ARG_TYPE);                                   \
            ASSIGN_TO = *data;                                                                     \
        }                                                                                          \
        break
