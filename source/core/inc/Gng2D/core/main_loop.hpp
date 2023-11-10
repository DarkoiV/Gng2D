#pragma once
#include <cstdint>

namespace Gng2D {
struct MainLoop
{
    void operator()();

  private:
    inline void loop();
    inline void eventsProcessing();
    inline void logicProcessing();
    inline void rendering();

    void switchScene();

    uint64_t previousTS{0};
    uint32_t logicLag{0};
};
} // namespace Gng2D
