#pragma once
#include <entt/entt.hpp>
#include "Gng2D/core/luna.hpp"
#include "Gng2D/core/scene_interface.hpp"
#include "Gng2D/scene/entity_renderer.hpp"

struct SDL_Renderer;

namespace Gng2D
{
struct Scene : SceneInterface 
{
    Scene();
    Scene(const Scene&) = delete;
    Scene(Scene&&)      = delete;
    virtual ~Scene()    = default;

    auto operator=(Scene&)  = delete;
    auto operator=(Scene&&) = delete;

    virtual void onEnter()  override;
    virtual void onExit()   override;
    virtual void update()   override;
    virtual void render(SDL_Renderer*) override;

    const std::string& getName() const override;

protected:
    entt::registry  reg;
    Luna            luna;

    std::string     name;
    EntityRenderer  entityRenderer{reg};
};
}

