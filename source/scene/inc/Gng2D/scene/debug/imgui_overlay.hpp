#pragma once
#include "Gng2D/commons/system_interface.hpp"
#include <functional>
#include <map>
#include <string>

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

  private:
    std::map<std::string, std::function<void()>> floatingWindows;

    void beginLeftSidebar();
    void endLeftSidebar();

    void entityList();
    void editedEntities();
};
} // namespace Gng2D
