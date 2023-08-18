#pragma once
#include <memory>
#include <vector>
#include <iterator>
#include <tuple>
#include <unordered_map>
#include <SDL2/SDL.h>
#include <entt/entt.hpp>
#include "Gng2D/types/coroutine.hpp"
#include "entity_renderer.hpp"
#include "position_system.hpp"
#include "mouse_system.hpp"

namespace Gng2D
{
struct Scene
{
    Scene();
    Scene(const Scene&) = delete;
    Scene(Scene&&)      = delete;
    virtual ~Scene();

    auto operator=(Scene&)  = delete;
    auto operator=(Scene&&) = delete;

    virtual void onEnter()      {};
    virtual void onExit()       {};
    virtual void preUpdate()    {};
    virtual void update()       {};
    virtual void render(SDL_Renderer*);

    virtual void onMouseMotion(SDL_MouseMotionEvent&);
    virtual void onMouseButton(SDL_MouseButtonEvent&);
    virtual void onKeyDown(SDL_KeyboardEvent&)  {};
    virtual void onKeyUp(SDL_KeyboardEvent&)    {};

    void operator()();
    bool isPaused()     const;
    bool isCompleted()  const;

    template<typename Coro, typename... Args>
    void addCoroutine(Coro coro, Args&&... args)
    {
        coroutines.emplace_back(coro, std::forward<Args>(args)...);
    }

    bool        isKeyPressed(SDL_Scancode) const;

protected:
    bool                pause{false};
    bool                runCoroutinesOnPause{false};
    bool                completed{false};
    entt::registry      reg;

    EntityRenderer      entityRenderer{reg};
    PositionSystem      positionSystem{reg};
    MouseSystem         mouseSystem{reg};

private:
    void                    runCoroutines();
    std::vector<Coroutine>  coroutines;

    static void addChildToParent(entt::registry&, entt::entity);
    static void removeChildFromParent(entt::registry&, entt::entity);
    static void destroyAllChildren(entt::registry&, entt::entity);
    static void updateChildrenLayer(entt::registry&, entt::entity);
};
}

