#include "Gng2D/scene/actions_handler.hpp"
#include "Gng2D/commons/log.hpp"

using Gng2D::ActionsHandler;

ActionsHandler::ActionsHandler(entt::registry& r)
    : reg(r)
{
    registerDefaultActions();

    reg.ctx().emplace<ActionsHandler&>(*this);
}

ActionsHandler::~ActionsHandler()
{
    reg.ctx().erase<ActionsHandler&>();
}

void ActionsHandler::onKeyPress(SDL_KeyboardEvent& event)
{
    auto it = keyActions.find(event.keysym.scancode);
    if (it == keyActions.end()) return;

    auto action = it->second;
    LOG::TRACE("Action:", action.data());
    actionsCallback.at(action).publish(reg, action);
}

void ActionsHandler::registerDefaultActions()
{
    using namespace entt::literals;
    keyActions.emplace(SDL_SCANCODE_UP, "up"_hs);
    keyActions.emplace(SDL_SCANCODE_DOWN, "down"_hs);
    keyActions.emplace(SDL_SCANCODE_RIGHT, "right"_hs);
    keyActions.emplace(SDL_SCANCODE_LEFT, "left"_hs);

    actionsCallback.emplace("up"_hs, ActionSigHandler{});
    actionsCallback.emplace("down"_hs, ActionSigHandler{});
    actionsCallback.emplace("right"_hs, ActionSigHandler{});
    actionsCallback.emplace("left"_hs, ActionSigHandler{});
}
