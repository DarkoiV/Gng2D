#pragma once
#include "Gng2D/commons/luna.hpp"
#include "Gng2D/commons/scene_interface.hpp"
#include "Gng2D/scene/assets.hpp"
#include "Gng2D/scene/entity_renderer.hpp"
#include "Gng2D/scene/input_handler.hpp"
#include <entt/entt.hpp>

struct SDL_Renderer;

namespace Gng2D
{
struct Scene : SceneInterface 
{
    Scene();
    Scene(const Scene&) = delete;
    Scene(Scene&&)      = delete;
    virtual ~Scene();

    auto operator=(Scene&)  = delete;
    auto operator=(Scene&&) = delete;

    virtual void onEnter()  override;
    virtual void onExit()   override;
    virtual void update()   override;
    virtual void render(SDL_Renderer*) override;

    virtual void onKeyPress(SDL_KeyboardEvent&) override;

    virtual const std::string& getName() const override;

protected:
    std::string     name;
    entt::registry  reg;

    Assets          assets;
    Luna            luna;
    InputHandler    inputHandler{reg};
    EntityRenderer  entityRenderer{reg};
};
}

