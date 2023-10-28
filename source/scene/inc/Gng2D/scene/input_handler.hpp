#pragma once
#include <entt/entt.hpp>
#include <SDL2/SDL.h>
#include <map>
#include "Gng2D/components/input.hpp"

namespace Gng2D
{
struct InputHandler
{
    InputHandler(entt::registry&);
    InputHandler(InputHandler&)     = delete;
    InputHandler(InputHandler&&)    = delete;
    ~InputHandler();

    auto operator=(InputHandler&)   = delete;
    auto operator=(InputHandler&&)  = delete;

    void handleKeyPress(SDL_KeyboardEvent&);

private:
    entt::registry& reg;
    std::map<SDL_Scancode, KeyPress::Id> keyPressActions;
};
}

