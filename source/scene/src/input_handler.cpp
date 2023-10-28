#include "Gng2D/scene/input_handler.hpp"
#include "Gng2D/components/input.hpp"
#include <algorithm>
#include <entt/entt.hpp>

using Gng2D::InputHandler;
using namespace entt::literals;

InputHandler::InputHandler(entt::registry& r)
    : reg(r)
{
    keyPressHandlers[SDL_SCANCODE_UP] = "up"_hs;
}

InputHandler::~InputHandler()
{
}

void InputHandler::handleKeyboardEvent(SDL_KeyboardEvent& e)
{
    if(e.repeat) return;

    auto view = reg.view<KeyboardInput>();
    view.each([this](entt::entity e, KeyboardInput& ki)
    {
        auto it = std::lower_bound(ki.handledInputs.begin(), ki.handledInputs.end(), "up"_hs);
        if (it != ki.handledInputs.end())
        {
            auto& inputHandler = *it;
            if (inputHandler.id != "up"_hs) return;
            inputHandler.handler(reg, e);
        }
    });
}

