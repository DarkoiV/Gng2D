#pragma once
#include "Gng2D/commons/assert.hpp"
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

    auto getActionSink(HashedString action)
    {
        GNG2D_ASSERT(actionsCallback.count(action), "Action not registered:", action.data());
        return entt::sink{actionsCallback.at(action)};
    }

  private:
    entt::registry& reg;

    std::map<SDL_Scancode, HashedString>     keyActions;
    std::map<HashedString, ActionSigHandler> actionsCallback;

    void registerDefaultActions();
};
} // namespace Gng2D
