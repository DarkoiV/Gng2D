#pragma once
#include "Gng2D/commons/luna/state.hpp"
#include "Gng2D/commons/scene_interface.hpp"
#include "Gng2D/commons/system_interface.hpp"
#include "Gng2D/entities/entity_recipe.hpp"
#include "Gng2D/scene/entity_renderer.hpp"
#include "Gng2D/scene/input_handler.hpp"
#include <entt/entt.hpp>
#include <filesystem>

namespace Gng2D {
struct Scene : SceneInterface
{
    Scene(const std::string&);
    Scene(const Scene&) = delete;
    Scene(Scene&&)      = delete;
    virtual ~Scene();

    auto operator=(Scene&)  = delete;
    auto operator=(Scene&&) = delete;

    virtual void onEnter() override;
    virtual void onExit() override;
    virtual void update() override;
    virtual void render(SDL_Renderer*) override;

    virtual void onKeyPress(SDL_KeyboardEvent&) override;
    virtual void onMouseMotion(SDL_MouseMotionEvent&) override { }
    virtual void onMouseButton(SDL_MouseButtonEvent&) override { }

    virtual const std::string& getName() const override;

  protected:
    const std::string           name;
    entt::registry              reg;
    const std::filesystem::path sceneDir;

    Luna::State& luna;
    InputHandler inputHandler{reg};

    EntityRenderer entityRenderer{reg};

    std::vector<SystemPtr>                        systems;
    std::unordered_map<std::string, EntityRecipe> entityRecipes;

  private:
    Luna::TableRef                   lunaSceneEnv;
    std::optional<Luna::FunctionRef> lunaOnEnter;
    std::optional<Luna::FunctionRef> lunaOnUpdate;

    int lunaSpawnEntity(Luna::Stack, Luna::TypeVector);
    int lunaNewEntityRecipe(Luna::Stack, Luna::TypeVector);
};
} // namespace Gng2D
