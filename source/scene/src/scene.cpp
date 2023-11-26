#include "Gng2D/scene/scene.hpp"
#include "Gng2D/commons/imgui.hpp"
#include "Gng2D/commons/log.hpp"
#include "Gng2D/commons/repository.hpp"
#include "Gng2D/core/global.hpp"
#include "Gng2D/entities/red_x.hpp"

#ifdef GNG2D_IMGUI_ENABLED
#include "Gng2D/scene/debug/imgui_overlay.hpp"
#endif

using Gng2D::Scene;

Scene::Scene()
    : luna(Gng2D::GLOBAL::LUNA_STATE)
    , lunaSceneEnv(luna.createTableRef())
{
#ifdef GNG2D_IMGUI_ENABLED
    systems.emplace_back(std::make_unique<Gng2D::ImguiOverlay>(reg));
#endif

    luna.doFile(GLOBAL::DATA_DIRECTORY / "scene.lua", lunaSceneEnv);

    auto OnEnterRef = lunaSceneEnv.get("OnEnter");
    if (OnEnterRef.isFunction()) lunaOnEnter = OnEnterRef.asFunction();
    else if (not OnEnterRef.isNil()) LOG::WARN("OnEnter should be a lua function!");

    auto OnUpdateRef = lunaSceneEnv.get("OnUpdate");
    if (OnUpdateRef.isFunction()) lunaOnUpdate = OnUpdateRef.asFunction();
    else if (not OnUpdateRef.isNil()) LOG::WARN("OnUpdate should be a lua function!");

    luna.registerMethod<&Scene::lunaSpawnEntity>(*this, "spawn", lunaSceneEnv);
}

Scene::~Scene()
{
    reg.clear();
}

void Scene::onEnter()
{
    using namespace entt::literals;
    LOG::INFO("Entering", name);

    Repository::loadSprite("red_x");
    entityRecipes.insert_or_assign("RedX"_hs, redX(&reg));

    if (lunaOnEnter) luna.getStack().callFunction(*lunaOnEnter);
}

void Scene::onExit()
{
    LOG::INFO("Exiting", name);
}

void Scene::update()
{
    for (auto& system: systems)
    {
        system->onUpdate();
    }

    if (lunaOnUpdate) luna.getStack().callFunction(*lunaOnUpdate);
}

void Scene::render(SDL_Renderer* r)
{
    entityRenderer(r);
}

void Scene::onKeyPress(SDL_KeyboardEvent& e)
{
    inputHandler.handleKeyPress(e);
}

const std::string& Scene::getName() const
{
    return name;
}

int Scene::lunaSpawnEntity(Luna::Stack stack, Luna::TypeVector args)
{
    GNG2D_ASSERT(args.size() == 1);
    auto entityName = args[0];
    if (not entityName.isString()) return 0;
    auto eid = entt::hashed_string::value(entityName.asString().c_str());

    auto recipeIt = entityRecipes.find(eid);
    if (recipeIt == entityRecipes.end()) return 0;
    recipeIt->second.spawn();

    return 0;
}
