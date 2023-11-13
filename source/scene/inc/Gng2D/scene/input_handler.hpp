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

    void registerKeyPressAction(SDL_Scancode, const InputListener::Action);

    void handleKeyPress(SDL_KeyboardEvent&);

  private:
    entt::registry&                               reg;
    std::map<SDL_Scancode, InputListener::Action> keyPressActions;
};
} // namespace Gng2D
