#pragma once
#include <entt/entt.hpp>

struct SDL_Renderer;

namespace Gng2D
{
struct Scene 
{
    Scene()             = default;
    Scene(const Scene&) = delete;
    Scene(Scene&&)      = delete;
    virtual ~Scene()    = default;

    auto operator=(Scene&)  = delete;
    auto operator=(Scene&&) = delete;

    virtual void onEnter()  {};
    virtual void onExit()   {};
    virtual void update()   {};
    virtual void render(SDL_Renderer*) {};

private:
    entt::registry reg;
};
}

