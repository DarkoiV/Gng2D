#include "Gng2D/scene/actions_handler.hpp"
#include "Gng2D/commons/log.hpp"
#include "Gng2D/commons/luna/state.hpp"
#include "Gng2D/components/hoverable.hpp"
#include "Gng2D/components/lua_script.hpp"
#include "Gng2D/components/transform.hpp"

using Gng2D::ActionsHandler;

#define HOVER_SORT_SIGNAL &ActionsHandler::markHoverablesForSorting

ActionsHandler::ActionsHandler(entt::registry& r)
    : reg(r)
{
    registerDefaultActions();

    // clang-format off
    entt::sigh_helper{reg}
        .with<detail::Layer>()
            .on_construct<HOVER_SORT_SIGNAL>(this)
            .on_update<HOVER_SORT_SIGNAL>(this)
            .on_destroy<HOVER_SORT_SIGNAL>(this);
    // clang-format on

    reg.ctx().emplace<ActionsHandler&>(*this);
    LOG::INFO("ActionsHandler created");
}

ActionsHandler::~ActionsHandler()
{
    reg.on_construct<detail::Layer>().disconnect(this);
    reg.on_update<detail::Layer>().disconnect(this);
    reg.on_destroy<detail::Layer>().disconnect(this);

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

void ActionsHandler::onMouseMotion(SDL_MouseMotionEvent& event)
{
    if (hoverablesNeedSorting) sortHoverables();
    auto view = reg.view<Hoverable, detail::Position>();

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

void ActionsHandler::markHoverablesForSorting()
{
    hoverablesNeedSorting = true;
}

void ActionsHandler::sortHoverables()
{
    reg.sort<Hoverable>(
        [&](entt::entity lhs, entt::entity rhs)
    {
        auto* leftLayer  = reg.try_get<detail::Layer>(lhs);
        auto* rightLayer = reg.try_get<detail::Layer>(rhs);

        if (not leftLayer and rightLayer) return false;
        if (not rightLayer and leftLayer) return true;

        return *leftLayer > *rightLayer;
    });
    hoverablesNeedSorting = false;
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

    // DIRECTIONAL KEY RELEASE
    keyReleaseActions.emplace(SDL_SCANCODE_UP, "rUp"_hs);
    keyReleaseActions.emplace(SDL_SCANCODE_DOWN, "rDown"_hs);
    keyReleaseActions.emplace(SDL_SCANCODE_RIGHT, "rRight"_hs);
    keyReleaseActions.emplace(SDL_SCANCODE_LEFT, "rLeft"_hs);

    actionsCallback.emplace("rUp"_hs, ActionSigHandler{});
    actionsCallback.emplace("rDown"_hs, ActionSigHandler{});
    actionsCallback.emplace("rRight"_hs, ActionSigHandler{});
    actionsCallback.emplace("rLeft"_hs, ActionSigHandler{});

    // CONFIRM / CANCELL
    keyPressActions.emplace(SDL_SCANCODE_RETURN, "confirm"_hs);
    keyPressActions.emplace(SDL_SCANCODE_ESCAPE, "cancel"_hs);
    actionsCallback.emplace("confirm"_hs, ActionSigHandler{});
    actionsCallback.emplace("cancel"_hs, ActionSigHandler{});
}
