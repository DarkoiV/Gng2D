#pragma once
#include "Gng2D/commons/system_interface.hpp"

struct SDL_KeyboardEvent;

namespace Gng2D {
struct ImguiOverlay : SystemInterface
{
    ImguiOverlay(entt::registry& r);
    ImguiOverlay(ImguiOverlay&)  = delete;
    ImguiOverlay(ImguiOverlay&&) = delete;
    virtual ~ImguiOverlay()      = default;

    auto operator=(ImguiOverlay&)  = delete;
    auto operator=(ImguiOverlay&&) = delete;

    virtual void onUpdate() override;

    bool catchKeyPress(SDL_KeyboardEvent&);
};
} // namespace Gng2D
