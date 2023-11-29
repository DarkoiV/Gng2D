#include "Gng2D/scene/scene.hpp"
#include "Gng2D/commons/imgui.hpp"
#include "Gng2D/commons/log.hpp"
#include "Gng2D/commons/repository.hpp"
#include "Gng2D/core/global.hpp"

#ifdef GNG2D_IMGUI_ENABLED
#include "Gng2D/scene/debug/imgui_overlay.hpp"
#define EMPLACE_IMGUI_SYSEM systems.emplace_back(std::make_unique<Gng2D::ImguiOverlay>(reg))
#else
#define EMPLACE_IMGUI_SYSEM (void(0))
#endif

using Gng2D::Scene;

Scene::Scene(const std::string& n)
    : lunaSceneEnv(luna.createTableRef())
    , name(n)
    , sceneDir(GLOBAL::DATA_DIRECTORY / ("scene_" + name))
{
    EMPLACE_IMGUI_SYSEM;
    Repository::attachComponentHooks(&reg);

    GNG2D_ASSERT(std::filesystem::is_directory(sceneDir));
    GNG2D_ASSERT(std::filesystem::is_regular_file(sceneDir / "scene.lua"));

    LOG::INFO("Loading scene data from:", sceneDir);
    luna.doFile(sceneDir / "scene.lua", lunaSceneEnv);

    reg.ctx().emplace<Luna::State&>(luna);

    auto OnEnterRef = lunaSceneEnv.get("OnEnter");
    if (OnEnterRef.isFunction()) lunaOnEnter = OnEnterRef.asFunction();
    else if (not OnEnterRef.isNil()) LOG::WARN("OnEnter should be a lua function!");

    auto OnUpdateRef = lunaSceneEnv.get("OnUpdate");
    if (OnUpdateRef.isFunction()) lunaOnUpdate = OnUpdateRef.asFunction();
    else if (not OnUpdateRef.isNil()) LOG::WARN("OnUpdate should be a lua function!");

    luna.registerMethod<&Scene::lunaSpawnEntity>(*this, "spawnEntity", lunaSceneEnv);
    luna.registerMethod<&Scene::lunaNewEntityRecipe>(*this, "newEntityRecipe", lunaSceneEnv);
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
    GNG2D_ASSERT(args.at(0).isString());
    auto& entityName = args.at(0).asString();

    auto recipeIt = entityRecipes.find(entityName);
    if (recipeIt == entityRecipes.end()) return 0;
    recipeIt->second.spawn();

    return 0;
}

int Scene::lunaNewEntityRecipe(Luna::Stack stack, Luna::TypeVector args)
{
    GNG2D_ASSERT(args.size() == 2 and args.at(0).isString() and args.at(1).isTable(),
                 "newEntityRecipe requires 2 args 1 of type string, and components table");
    auto& entityName      = args.at(0).asString();
    auto& componentsTable = args.at(1).asTable();
    LOG::INFO("Registering entity from lua:", args.at(0).asString());

    EntityRecipe recipe(&reg);
    for (auto&& [compName, compArgs]: componentsTable)
    {
        if (not compName.isInteger() and not compName.isString())
        {
            LOG::ERROR("in lunaNewEntityRecipe, key of table has to be string or hash");
            continue;
        }
        if (not compArgs.isTable())
        {
            LOG::ERROR("in lunaNewEntityRecipe, value of componentArgs has to be table");
            continue;
        }
        StringHash hash = compName.isInteger()
                            ? compName.asInteger()
                            : entt::hashed_string::value(compName.asString().c_str());

        auto type = entt::resolve(hash);
        if (not type)
        {
            LOG::ERROR("Faile to resolve:",
                       compName.isString() ? compName.asString() : std::to_string(hash));
            continue;
        }
        LOG::TRACE("Resolved to component:", type.info().name(), hash);
        recipe.addComponent(hash, compArgs.asTable());
        LOG::TRACE("Component added");
    }
    entityRecipes.insert_or_assign(entityName, std::move(recipe));
    LOG::OK("Created recipe for Entity:", entityName);
    return 0;
}
