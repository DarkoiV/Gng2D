#include "Gng2D/scene/input_handler.hpp"
#include "Gng2D/components/input.hpp"
#include <algorithm>
#include <entt/entt.hpp>
#include "Gng2D/commons/log.hpp"

using Gng2D::InputHandler;
using namespace entt::literals;

InputHandler::InputHandler(entt::registry& r)
    : reg(r)
{
}

InputHandler::~InputHandler()
{
}

void InputHandler::registerKeyPressAction(SDL_Scancode scancode, KeyPress::Action action)
{
    keyPressActions[scancode] = action;
}

void InputHandler::handleKeyPress(SDL_KeyboardEvent& e)
{
    if(e.repeat) return;
    auto actionIt = keyPressActions.find(e.keysym.scancode);
    if (actionIt == keyPressActions.end()) return;

    auto action = actionIt->second;
    auto view = reg.view<KeyPress>();
    view.each([this, action](entt::entity e, KeyPress& k)
    {
        auto it = std::find(k.actions.begin(), k.actions.end(), action);
        if (it == k.actions.end()) return;
        k.signal.publish(e, action);
    });
}
