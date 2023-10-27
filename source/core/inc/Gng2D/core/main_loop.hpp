#pragma once
#include <cstdint>

namespace Gng2D
{
struct MainLoop
{
    void operator()();

    inline void eventsProcessing();
    inline void logicProcessing();
    inline void rendering();

private:
    uint64_t    previousTS{0};
    uint32_t    logicLag{0};
};
}

