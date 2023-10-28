#include "Gng2D/scene/input_handler.hpp"
#include "Gng2D/components/input.hpp"
#include <algorithm>
#include <entt/entt.hpp>

using Gng2D::InputHandler;
using namespace entt::literals;

InputHandler::InputHandler(entt::registry& r)
    : reg(r)
{
    keyPressActions[SDL_SCANCODE_UP] = "up"_hs;
    keyPressActions[SDL_SCANCODE_W]  = "up"_hs;
}

InputHandler::~InputHandler()
{
}

void InputHandler::handleKeyPress(SDL_KeyboardEvent& e)
{
    if(e.repeat) return;

    auto action = keyPressActions.find(e.keysym.scancode);
    if (action == keyPressActions.end()) return;

    auto view = reg.view<KeyPress>();
    view.each([this, action](entt::entity e, KeyPress& ki)
    {
        auto it = std::lower_bound(ki.handlers.begin(), ki.handlers.end(), action->second);
        if (it != ki.handlers.end())
        {
            auto& inputHandler = *it;
            if (inputHandler.id != "up"_hs) return;
            inputHandler.handler(reg, e);
        }
    });
}

