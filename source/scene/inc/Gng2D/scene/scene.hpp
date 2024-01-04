#pragma once
#include "Gng2D/commons/luna/state.hpp"
#include "Gng2D/commons/scene_interface.hpp"
#include "Gng2D/commons/system_interface.hpp"
#include "Gng2D/entities/lua_api.hpp"
#include "Gng2D/entities/recipe.hpp"
#include "Gng2D/scene/actions_handler.hpp"
#include "Gng2D/scene/collision_system.hpp"
#include "Gng2D/scene/entity_renderer.hpp"
#include <entt/entt.hpp>
#include <filesystem>

namespace Gng2D {
struct Scene : SceneInterface
{
    Scene(const std::string&, const std::filesystem::path&);
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
    virtual void onKeyRelease(SDL_KeyboardEvent&) override;
    virtual void onMouseMotion(SDL_MouseMotionEvent&) override;
    virtual void onMouseButton(SDL_MouseButtonEvent&) override { }

    virtual const std::string& getName() const override;

  protected:
    entt::registry              reg;
    const std::string           name;
    const std::filesystem::path sceneDir;

    Luna::State     luna;
    ActionsHandler  actionsHandler{reg};
    CollisionSystem collisionSystem{reg};
    EntityLuaApi    entityLuaApi{reg, luna};

    EntityRenderer entityRenderer{reg};

    std::vector<SystemPtr>                        systems;
    std::unordered_map<std::string, EntityRecipe> entityRecipes;

  private:
    entt::sigh<void(entt::registry&, entt::entity)> onSpawnSignal;

    void insertSignalsIntoCtx();

  private:
    Luna::TableRef                   lunaSceneEnv;
    std::optional<Luna::FunctionRef> lunaOnEnter;
    std::optional<Luna::FunctionRef> lunaOnUpdate;

    void initLunaEnv();
    void registerLunaMethods();
    void registerLunaOnAction();
    void invokeAction(entt::registry&, HashedString);

    int lunaSpawnEntity(Luna::Stack, Luna::TypeVector);
    int lunaNewEntityRecipe(Luna::Stack, Luna::TypeVector);
    int lunaViewEach(Luna::Stack, Luna::TypeVector);
};
} // namespace Gng2D
