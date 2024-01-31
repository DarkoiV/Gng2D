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

    void registerOnKeyPressAction(SDL_Scancode, HashedString);
    void registerOnKeyReleaseAction(SDL_Scancode, HashedString);

    auto emit(HashedString action) { actionsCallback[action].publish(reg, action); }
    auto getActionSink(HashedString action) { return entt::sink{actionsCallback[action]}; }

  private:
    entt::registry& reg;

    bool hoverablesNeedSorting{false};
    void markHoverablesForSorting();
    void sortHoverables();

    entt::entity currentHover = entt::null;
    void         enterHover(entt::entity);
    void         leaveHover(entt::entity);

    std::map<SDL_Scancode, HashedString>     keyPressActions;
    std::map<SDL_Scancode, HashedString>     keyReleaseActions;
    std::map<HashedString, ActionSigHandler> actionsCallback;

    void registerDefaultActions();
};
} // namespace Gng2D
