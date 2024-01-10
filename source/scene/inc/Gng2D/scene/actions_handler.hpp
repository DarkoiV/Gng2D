#pragma once
#include "Gng2D/commons/types.hpp"
#include <SDL2/SDL_events.h>

namespace Gng2D {
struct ActionsHandler
{
    using ActionSigHandler = entt::sigh<void(entt::registry&, HashedString)>;

    ActionsHandler(entt::registry&);
    ActionsHandler(ActionsHandler&)  = delete;
    ActionsHandler(ActionsHandler&&) = delete;
    ~ActionsHandler();

    auto operator=(ActionsHandler&)  = delete;
    auto operator=(ActionsHandler&&) = delete;

    void onKeyPress(SDL_KeyboardEvent&);
    void onKeyRelease(SDL_KeyboardEvent&);
    void onMouseMotion(SDL_MouseMotionEvent&);

    auto emit(HashedString action) { actionsCallback[action].publish(reg, action); }
    auto getActionSink(HashedString action) { return entt::sink{actionsCallback[action]}; }

  private:
    entt::registry& reg;

    bool hoverablesNeedSorting{false};
    void markHoverablesForSorting();
    void sortHoverables();

    std::map<SDL_Scancode, HashedString>     keyPressActions;
    std::map<SDL_Scancode, HashedString>     keyReleaseActions;
    std::map<HashedString, ActionSigHandler> actionsCallback;

    void registerDefaultActions();
};
} // namespace Gng2D
