#pragma once
#include "Gng2D/commons/scene_interface.hpp"
#include "entt/entity/fwd.hpp"

namespace Gng2D {
struct Editor : SceneInterface
{
    void onEnter() final;
    void onExit() final;
    void update() final;
    void render(SDL_Renderer*) final;

    void         onKeyPress(SDL_KeyboardEvent&) final;
    virtual void onMouseMotion(SDL_MouseMotionEvent&) final;
    virtual void onMouseButton(SDL_MouseButtonEvent&) final;

    const std::string& getName() const final;

  private:
    std::string name{"Editor"};
    ScenePtr    editedScene;

    entt::registry* sceneReg{};
};
} // namespace Gng2D
