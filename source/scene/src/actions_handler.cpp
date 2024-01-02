#include "Gng2D/scene/actions_handler.hpp"
#include "Gng2D/commons/log.hpp"
#include "Gng2D/components/collider.hpp"
#include "Gng2D/components/lua_script.hpp"
#include "Gng2D/components/transform.hpp"
#include "Gng2D/core/global.hpp"

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
    actionsCallback.at(action).publish(reg, action, false);
}

void ActionsHandler::onKeyRelease(SDL_KeyboardEvent& event)
{
    auto it = keyPressActions.find(event.keysym.scancode);
    if (it == keyPressActions.end()) return;

    auto action = it->second;
    LOG::TRACE("Action release:", action.data());
    actionsCallback.at(action).publish(reg, action, true);
}

void ActionsHandler::onMouseMotion(SDL_MouseMotionEvent& event)
{
    auto&&           mouseColliderStorage = reg.storage<detail::BoxCollider>("mouse"_hash);
    entt::basic_view view{mouseColliderStorage, reg.storage<detail::Position>()};

    for (auto&& [e, box, pos]: view.each())
    {
        auto hw = box.width / 2;
        auto hh = box.height / 2;
        if (event.x > (pos.x - hw) and event.x < (pos.x + hw) and event.y > (pos.y - hh) and
            event.y < (pos.y + hh))
        {
            if (auto* luaScript = reg.try_get<LuaScript>(e))
            {
                if (not luaScript->hasOnHover) return;

                auto onHover = luaScript->entityEnv.get("OnHover").asFunction();
                auto stack   = reg.ctx().get<Luna::State>().getStack();
                stack.callFunction(onHover);
                LOG::TRACE("Hover:", (unsigned)e);
            }
            return;
        }
    }
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

    // CONFIRM / CANCELL
    keyPressActions.emplace(SDL_SCANCODE_RETURN, "confirm"_hs);
    keyPressActions.emplace(SDL_SCANCODE_ESCAPE, "cancel"_hs);
    actionsCallback.emplace("confirm"_hs, ActionSigHandler{});
    actionsCallback.emplace("cancel"_hs, ActionSigHandler{});
}
