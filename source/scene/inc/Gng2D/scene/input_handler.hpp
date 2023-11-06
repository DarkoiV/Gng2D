#pragma once
#include "Gng2D/components/input.hpp"
#include <SDL2/SDL.h>
#include <entt/entt.hpp>
#include <map>

namespace Gng2D {
struct InputHandler
{
    InputHandler(entt::registry&);
    InputHandler(InputHandler&)  = delete;
    InputHandler(InputHandler&&) = delete;
    ~InputHandler();

    auto operator=(InputHandler&)  = delete;
    auto operator=(InputHandler&&) = delete;

    void registerKeyPressAction(SDL_Scancode, const ActionListener::ActionString&);

    void handleKeyPress(SDL_KeyboardEvent&);

  private:
    entt::registry&                                      reg;
    std::map<SDL_Scancode, ActionListener::ActionString> keyPressActions;
};
} // namespace Gng2D
