#include "Gng2D/scene/scene.hpp"
#include "Gng2D/commons/imgui.hpp"
#include "Gng2D/commons/log.hpp"
#include "Gng2D/components/lua_script.hpp"

#ifdef GNG2D_IMGUI_ENABLED
#include "Gng2D/scene/debug/imgui_overlay.hpp"
#define EMPLACE_IMGUI_SYSEM() systems.emplace_back(std::make_unique<Gng2D::ImguiOverlay>(reg))
#else
#define EMPLACE_IMGUI_SYSEM() (void(0))
#endif

using Gng2D::Scene;

Scene::Scene(const std::string& n, const std::filesystem::path& dir)
    : lunaSceneEnv(luna.createTableRef())
    , name(n)
    , sceneDir(dir)
    , onActionMetatable(luna.createTableRef())
{
    EMPLACE_IMGUI_SYSEM();
    insertSignalsIntoCtx();
    repository.attachComponentHooks(&reg);

    GNG2D_ASSERT(std::filesystem::is_directory(sceneDir));
    GNG2D_ASSERT(std::filesystem::is_regular_file(sceneDir / "scene.lua"));

    LOG::INFO("Loading scene data from:", sceneDir);

    initLunaEnv();
    registerLunaMethods();
    registerLunaOnAction();
    loadRecipes();

    reg.ctx().emplace<Luna::State&>(luna);
    reg.ctx().emplace_as<Luna::TableRef&>("sceneEnv"_hash, lunaSceneEnv);
    reg.ctx().emplace<SceneRepository&>(repository);
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

void Scene::onKeyPress(SDL_KeyboardEvent& e)
{
    actionsHandler.onKeyPress(e);
}

void Scene::onKeyRelease(SDL_KeyboardEvent& e)
{
    actionsHandler.onKeyRelease(e);
}

void Scene::onMouseMotion(SDL_MouseMotionEvent& e)
{
    actionsHandler.onMouseMotion(e);
}

const std::string& Scene::getName() const
{
    return name;
}

void Scene::insertSignalsIntoCtx()
{
    reg.ctx().emplace_as<CompSig>(CompSigHook::ON_SPAWN, onSpawnSignal);
}

void Scene::initLunaEnv()
{
    lunaSceneEnv.createSubTable("OnAction");
    luna.doFile(sceneDir / "scene.lua", lunaSceneEnv);
}

void Scene::registerLunaMethods()
{
    auto OnEnterRef = lunaSceneEnv.get("OnEnter");
    if (OnEnterRef.isFunction()) lunaOnEnter = OnEnterRef.asFunction();
    else if (not OnEnterRef.isNil()) LOG::WARN("OnEnter should be a lua function!");

    auto OnUpdateRef = lunaSceneEnv.get("OnUpdate");
    if (OnUpdateRef.isFunction()) lunaOnUpdate = OnUpdateRef.asFunction();
    else if (not OnUpdateRef.isNil()) LOG::WARN("OnUpdate should be a lua function!");

    luna.registerMethod<&Scene::lunaSpawnEntity>(*this, "SpawnEntity", lunaSceneEnv);
    luna.registerMethod<&Scene::lunaNewEntityRecipe>(*this, "NewEntityRecipe", lunaSceneEnv);
    luna.registerMethod<&Scene::lunaViewEach>(*this, "ViewEach", lunaSceneEnv);
    luna.registerMethod<&Scene::lunaEmitAction>(*this, "EmitAction", lunaSceneEnv);
    luna.registerMethod<&Scene::lunaRegisterOnKeyPressAction>(*this, "RegisterOnKeyPressAction",
                                                              lunaSceneEnv);
    luna.registerMethod<&Scene::lunaRegisterOnKeyReleaseAction>(*this, "RegisterOnKeyReleaseAction",
                                                                lunaSceneEnv);
}

void Scene::registerLunaOnAction()
{
    auto onAction = lunaSceneEnv.get("OnAction");
    lunaSceneEnv.set("OnAction", Luna::Nil{});
    lunaSceneEnv.createSubTable("OnAction");
    auto onActionProxy = lunaSceneEnv.get("OnAction").asTable();

    luna.registerMethod<&Scene::onAction__newindex>(*this, "__newindex", onActionMetatable);
    luna.registerMethod<&Scene::onAction__index>(*this, "__index", onActionMetatable);
    onActionProxy.setMetaTable(onActionMetatable);

    if (not onAction.isTable())
    {
        onActionProxy.createSubTable("functions"_hash);
        return;
    }
    onActionProxy.set("functions"_hash, onAction);

    for (auto&& [action, callback]: onAction.asTable())
    {
        GNG2D_ASSERT(action.isString() and callback.isFunction());
        auto actionHS   = action.asHashedString();
        auto sink       = actionsHandler.getActionSink(actionHS);
        auto connection = sink.connect<&Scene::invokeAction>(*this);
        onActionConnection.emplace(actionHS, connection);
    }
}

void Scene::loadRecipes()
{
    auto& globalRecipes = repository.accessEntityRecipes();
    for (auto&& [name, recipe]: globalRecipes)
    {
        auto recipeTable = luna.createTableRef();
        luna.doFile(recipe, recipeTable);
        lunaNewEntityRecipe(luna.getStack(), {name, recipeTable});
    }
}

void Scene::invokeAction(entt::registry&, HashedString action)
{
    auto stack    = luna.getStack();
    auto onAction = lunaSceneEnv.get("OnAction").asTable().get("functions"_hash);
    auto callback = onAction.asTable().get(action.data());
    stack.callFunction(callback.asFunction());
}

int Scene::onAction__index(Luna::Stack stack, Luna::TypeVector args)
{
    constexpr auto ARGS_ERROR =
        "OnAction __index requires 2 arguments, "
        "first should be onAction proxy table, "
        "second should be action name";
    GNG2D_ASSERT(args.size() == 2, ARGS_ERROR);
    GNG2D_ASSERT(args.at(0).isTable(), ARGS_ERROR);
    GNG2D_ASSERT(args.at(1).isString(), ARGS_ERROR);

    auto onAction = args.at(0).asTable().get("functions"_hash).asTable();

    stack.push(onAction.get(args.at(1)));

    return 1;
}

int Scene::onAction__newindex(Luna::Stack stack, Luna::TypeVector args)
{
    constexpr auto ARGS_ERROR =
        "OnAction __newindex requires 3 arguments, "
        "first should be onAction proxy table, "
        "second should be action name, "
        "third should be function or nil";
    GNG2D_ASSERT(args.size() == 3, ARGS_ERROR);
    GNG2D_ASSERT(args.at(0).isTable(), ARGS_ERROR);
    GNG2D_ASSERT(args.at(1).isString(), ARGS_ERROR);

    auto onAction = args.at(0).asTable().get("functions"_hash).asTable();

    if (args.at(2).isNil())
    {
        onAction.set(args.at(1), Luna::Nil{});
        auto actionHS = args.at(1).asHashedString();
        onActionConnection.erase(actionHS);
    }
    else if (args.at(2).isFunction())
    {
        onAction.set(args.at(1), args.at(2));
        auto actionHS                = args.at(1).asHashedString();
        auto sink                    = actionsHandler.getActionSink(actionHS);
        auto connection              = sink.connect<&Scene::invokeAction>(*this);
        onActionConnection[actionHS] = connection;
    }
    else [[unlikely]]
    {
        LOG::ERROR(ARGS_ERROR);
    }

    return 0;
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
        LOG::ERROR("Did not found entity recipe:", entityName);
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
                LOG::ERROR("Failed to resolve:", compName.asString());
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

int Scene::lunaEmitAction(Luna::Stack stack, Luna::TypeVector args)
{
    constexpr auto ARGS_ERROR =
        "EmitAction requries one argument,\n"
        "it being action name to emit";
    GNG2D_ASSERT(args.size() == 1, ARGS_ERROR);
    GNG2D_ASSERT(args.at(0).isString(), ARGS_ERROR);

    actionsHandler.emit(args.at(0).asHashedString());

    return 0;
}

int Scene::lunaRegisterOnKeyPressAction(Luna::Stack stack, Luna::TypeVector args)
{
    constexpr auto ARGS_ERROR =
        "RegisterOnKeyPressAction requires two argumens, \n"
        "first being scancode name,\n"
        "second being action name";
    GNG2D_ASSERT(args.size() == 2, ARGS_ERROR);
    GNG2D_ASSERT(args.at(0).isString(), ARGS_ERROR);
    GNG2D_ASSERT(args.at(1).isString(), ARGS_ERROR);

    auto scancode = SDL_GetScancodeFromName(args.at(0).asString().c_str());
    if (scancode == SDL_SCANCODE_UNKNOWN) [[unlikely]]
    {
        LOG::ERROR(args.at(0).asString(), "is unknown scancode");
    }
    else
    {
        actionsHandler.registerOnKeyPressAction(scancode, args.at(1).asHashedString());
    }

    return 0;
}

int Scene::lunaRegisterOnKeyReleaseAction(Luna::Stack stack, Luna::TypeVector args)
{
    constexpr auto ARGS_ERROR =
        "RegisterOnKeyReleaseAction requires two argumens, \n"
        "first being scancode name,\n"
        "second being action name";
    GNG2D_ASSERT(args.size() == 2, ARGS_ERROR);
    GNG2D_ASSERT(args.at(0).isString(), ARGS_ERROR);
    GNG2D_ASSERT(args.at(1).isString(), ARGS_ERROR);

    auto scancode = SDL_GetScancodeFromName(args.at(0).asString().c_str());
    if (scancode == SDL_SCANCODE_UNKNOWN) [[unlikely]]
    {
        LOG::ERROR(args.at(0).asString(), "is unknown scancode");
    }
    else
    {
        LOG::INFO(args.at(1).asString());
        actionsHandler.registerOnKeyReleaseAction(scancode, args.at(1).asHashedString());
    }

    return 0;
}
