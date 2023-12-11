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
    auto it = keyPressActions.find(event.keysym.scancode);
    if (it == keyPressActions.end()) return;

    auto action = it->second;
    LOG::TRACE("Action:", action.data());
    actionsCallback.at(action).publish(reg, action);
}

void ActionsHandler::onKeyRelease(SDL_KeyboardEvent& event)
{
    auto it = keyReleaseActions.find(event.keysym.scancode);
    if (it == keyReleaseActions.end()) return;

    auto action = it->second;
    LOG::TRACE("Action:", action.data());
    actionsCallback.at(action).publish(reg, action);
}

void ActionsHandler::registerDefaultActions()
{
    using namespace entt::literals;
    // DIRECTIONAL ARROW PRESS
    keyPressActions.emplace(SDL_SCANCODE_UP, "up"_hs);
    keyPressActions.emplace(SDL_SCANCODE_DOWN, "down"_hs);
    keyPressActions.emplace(SDL_SCANCODE_RIGHT, "right"_hs);
    keyPressActions.emplace(SDL_SCANCODE_LEFT, "left"_hs);

    actionsCallback.emplace("up"_hs, ActionSigHandler{});
    actionsCallback.emplace("down"_hs, ActionSigHandler{});
    actionsCallback.emplace("right"_hs, ActionSigHandler{});
    actionsCallback.emplace("left"_hs, ActionSigHandler{});

    // DIRECTIONAL ARROW RELEASE
    keyReleaseActions.emplace(SDL_SCANCODE_UP, "upRelease"_hs);
    keyReleaseActions.emplace(SDL_SCANCODE_DOWN, "downRelease"_hs);
    keyReleaseActions.emplace(SDL_SCANCODE_RIGHT, "rightRelease"_hs);
    keyReleaseActions.emplace(SDL_SCANCODE_LEFT, "leftRelease"_hs);

    actionsCallback.emplace("upRelease"_hs, ActionSigHandler{});
    actionsCallback.emplace("downRelease"_hs, ActionSigHandler{});
    actionsCallback.emplace("rightRelease"_hs, ActionSigHandler{});
    actionsCallback.emplace("leftRelease"_hs, ActionSigHandler{});

    // CONFIRM / CANCELL
    keyPressActions.emplace(SDL_SCANCODE_RETURN, "confirm"_hs);
    keyPressActions.emplace(SDL_SCANCODE_ESCAPE, "cancel"_hs);
    actionsCallback.emplace("confirm"_hs, ActionSigHandler{});
    actionsCallback.emplace("cancel"_hs, ActionSigHandler{});
}
