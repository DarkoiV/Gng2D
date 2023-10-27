#pragma once
#include <entt/entt.hpp>
#include "Gng2D/core/luna.hpp"
#include "Gng2D/scene/entity_renderer.hpp"

struct SDL_Renderer;

namespace Gng2D
{
struct Scene 
{
    Scene();
    Scene(const Scene&) = delete;
    Scene(Scene&&)      = delete;
    virtual ~Scene()    = default;

    auto operator=(Scene&)  = delete;
    auto operator=(Scene&&) = delete;

    virtual void onEnter();
    virtual void onExit();
    virtual void update();
    virtual void render(SDL_Renderer*);

    const std::string& getName() const;

protected:
    entt::registry  reg;
    Luna            luna;

    std::string     name;
    EntityRenderer  entityRenderer{reg};
};
}

