#include "Gng2D/scene/scene.hpp"
#include "Gng2D/commons/imgui.hpp"
#include "Gng2D/commons/log.hpp"
#include "Gng2D/commons/repository.hpp"
#include "Gng2D/components/lua_script.hpp"

#ifdef GNG2D_IMGUI_ENABLED
#include "Gng2D/scene/debug/imgui_overlay.hpp"
#define EMPLACE_IMGUI_SYSEM systems.emplace_back(std::make_unique<Gng2D::ImguiOverlay>(reg))
#else
#define EMPLACE_IMGUI_SYSEM (void(0))
#endif

using Gng2D::Scene;

Scene::Scene(const std::string& n, const std::filesystem::path& dir)
    : lunaSceneEnv(luna.createTableRef())
    , name(n)
    , sceneDir(dir)
{
    EMPLACE_IMGUI_SYSEM;
    insertSignalsIntoCtx();
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

    luna.registerMethod<&Scene::lunaSpawnEntity>(*this, "SpawnEntity", lunaSceneEnv);
    luna.registerMethod<&Scene::lunaNewEntityRecipe>(*this, "NewEntityRecipe", lunaSceneEnv);
    luna.registerMethod<&Scene::lunaViewEach>(*this, "ViewEach", lunaSceneEnv);
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

    collisionSystem.onUpdate();
    entityLuaApi.onUpdate();

    if (lunaOnUpdate) luna.getStack().callFunction(*lunaOnUpdate);
}

void Scene::render(SDL_Renderer* r)
{
    entityRenderer(r);
}

void Scene::onKeyPress(SDL_KeyboardEvent& e) { }

const std::string& Scene::getName() const
{
    return name;
}

void Scene::insertSignalsIntoCtx()
{
    reg.ctx().emplace_as<CompSig>(CompSigHook::ON_SPAWN, onSpawnSignal);
}

int Scene::lunaSpawnEntity(Luna::Stack stack, Luna::TypeVector args)
{
    GNG2D_ASSERT(args.size() == 1 or args.size() == 2,
                 "spawnEntity requires 1 argument with optional 2nd, "
                 "first is name of entity, "
                 "second can be components table");
    GNG2D_ASSERT(args.at(0).isString());
    auto& entityName = args.at(0).asString();

    auto recipeIt = entityRecipes.find(entityName);
    if (recipeIt == entityRecipes.end())
    {
        LOG::ERROR("Did not find entity recipe:", entityName);
        return 0;
    }
    auto e = recipeIt->second.spawn();

    if (args.size() == 2 and args.at(1).isTable())
    {
        auto& componentsTable = args.at(1).asTable();
        for (const auto&& [compName, compArgs]: componentsTable)
        {
            if (not compArgs.isTable())
            {
                LOG::ERROR("in lunaSpawnEntity, value of componentArgs has to be table");
                continue;
            }
            StringHash hash = compName.asStringHash();
            auto       type = entt::resolve(hash);
            if (not type)
            {
                LOG::ERROR("Faile to resolve:", compName.asString());
                continue;
            }
            LOG::TRACE("Resolved to component:", type.info().name(), hash);
            auto       emplace = type.func("emplace"_hash);
            ArgsVector av      = compArgs.asTable();
            if (not emplace or not emplace.invoke({}, &reg, e, &av))
            {
                LOG::ERROR("Failed to resolve or call emplace function");
                continue;
            }
            LOG::TRACE("Component added");
        }
    }

    onSpawnSignal.publish(reg, e);
    return 0;
}

int Scene::lunaNewEntityRecipe(Luna::Stack stack, Luna::TypeVector args)
{
    GNG2D_ASSERT(args.size() == 2 and args.at(0).isString() and args.at(1).isTable(),
                 "newEntityRecipe requires 2 args, "
                 "first is name of entity, "
                 "second is components table");
    auto& entityName      = args.at(0).asString();
    auto& componentsTable = args.at(1).asTable();
    LOG::INFO("Registering entity from lua:", args.at(0).asString());

    EntityRecipe recipe(&reg);
    for (auto&& [compName, compArgs]: componentsTable)
    {
        if (not compName.isString())
        {
            LOG::ERROR("in lunaNewEntityRecipe, component name has to be string");
            continue;
        }
        if (not compArgs.isTable())
        {
            LOG::ERROR("in lunaNewEntityRecipe, value of componentArgs has to be table");
            continue;
        }
        StringHash hash = compName.asStringHash();
        auto       type = entt::resolve(hash);
        if (not type)
        {
            LOG::ERROR("Faile to resolve:", compName.asString());
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

int Scene::lunaViewEach(Luna::Stack stack, Luna::TypeVector args)
{
    constexpr auto ARGS_ERROR =
        "ViewEach requries at least one argument,\n"
        "first one being callback to call on each entity + component pack\n"
        "after that, zero or more component names for component pack";
    GNG2D_ASSERT(args.size() > 0, ARGS_ERROR);
    GNG2D_ASSERT(args.at(0).isFunction(), ARGS_ERROR);
    GNG2D_ASSERT(std::all_of(args.begin() + 1, args.end(), [](auto& t) { return t.isString(); }),
                 ARGS_ERROR);

    std::vector<entt::meta_type> compTypes;
    compTypes.reserve(args.size() - 1);

    entt::runtime_view view{};
    if (args.size() == 1) view.iterate(reg.storage<entt::entity>());
    else
    {
        for (int i = 1; i < args.size(); i++)
        {
            auto compType = entt::resolve(args.at(i).asStringHash());
            GNG2D_ASSERT(compType, "Failed to resolve:", args.at(i).asString());
            compTypes.push_back(compType);
            auto* storage = reg.storage(compType.info().hash());
            GNG2D_ASSERT(storage, "Failed to find storage for", args.at(i).asString());
            view.iterate(*storage);
        }
    }

    auto pushComponents = [&](Luna::Stack& stack, entt::entity e)
    {
        for (auto& compType: compTypes)
        {
            entityLuaApi.pushComponent(stack, e, compType);
        }
    };

    for (auto e: view)
    {
        stack.subscope();
        stack.push(args.at(0));
        stack.newTable();
        auto entityTable = stack.read(-1).asTable();
        entityLuaApi.setEntityTable(e, entityTable);
        pushComponents(stack, e);
        stack.callFunctionFS(args.size()); // Comp pack size (which is args - 1) and entity table
    }

    return 0;
}
