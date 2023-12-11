#include "Gng2D/entities/lua_api.hpp"
#include "Gng2D/commons/args_vector.hpp"
#include "Gng2D/components/lua_script.hpp"
#include "Gng2D/components/meta/component_userdata.hpp"
#include "Gng2D/components/meta/properties.hpp"
#include "Gng2D/components/meta/util_funcs.hpp"

using Gng2D::EntityLuaApi;
using LOG = Gng2D::LOG;
using namespace entt::literals;
using CompUserdata = Gng2D::ComponentUserdata;

EntityLuaApi::EntityLuaApi(entt::registry& r, Luna::State& ls)
    : SystemInterface(r)
    , lunaState(ls)
    , apiTable(lunaState.createTableRef())
{
    apiTable.createSubTable("componentMeta");
    auto compMetaTable = apiTable.get("componentMeta").asTable();

    constexpr auto comp_idx  = &EntityLuaApi::component__index;
    constexpr auto comp_nidx = &EntityLuaApi::component__newindex;
    lunaState.registerMethod<comp_idx>(*this, "__index", compMetaTable);
    lunaState.registerMethod<comp_nidx>(*this, "__newindex", compMetaTable);

    reg.ctx().emplace<EntityLuaApi&>(*this);
}

EntityLuaApi::~EntityLuaApi()
{
    reg.ctx().erase<EntityLuaApi&>();
}

void EntityLuaApi::onUpdate()
{
    for (auto&& [e, script]: reg.view<LuaScript>().each())
    {
        if (auto onUpdate = script.entityEnv.get("OnUpdate"); onUpdate.isFunction())
        {
            lunaState.getStack().callFunction(onUpdate.asFunction());
        }
    }
}

void EntityLuaApi::setEntityTable(entt::entity e, Luna::TableRef& env)
{
    env.set("Self", env);
    env.set("entity"_hash, (Luna::Integer)e);

    lunaState.registerMethod<&EntityLuaApi::getComponent>(*this, "getComponent", env);
    lunaState.registerMethod<&EntityLuaApi::addComponent>(*this, "addComponent", env);
    lunaState.registerMethod<&EntityLuaApi::hasComponent>(*this, "hasComponent", env);
}

void EntityLuaApi::pushComponent(Luna::Stack& stack, entt::entity e, entt::meta_type type)
{
    auto getRef = type.func("getRef"_hs);
    GNG2D_ASSERT(getRef);

    auto component = stack.newUserdata<ComponentUserdata>(e, getRef.invoke({}, &reg, e));
    component.setMetaTable(apiTable.get("componentMeta").asTable());
}

int EntityLuaApi::addComponent(Luna::Stack, Luna::TypeVector args)
{
    constexpr auto ARGS_ERROR =
        "add component requires 3 arguments, "
        "first should be entity table, "
        "second should be component name, "
        "third should be table with args";
    GNG2D_ASSERT(args.size() == 3, ARGS_ERROR);
    GNG2D_ASSERT(args.at(0).isTable(), ARGS_ERROR);
    GNG2D_ASSERT(args.at(1).isString(), ARGS_ERROR);
    GNG2D_ASSERT(args.at(2).isTable(), ARGS_ERROR);

    auto& enttable = args.at(0).asTable();
    auto  eid      = (entt::entity)enttable.get("entity"_hash).asInteger();
    auto  compHash = args.at(1).asStringHash();
    auto& compArgs = args.at(2).asTable();

    auto compType = entt::resolve(compHash);
    if (not compType) LOG::ERROR("Failed to resolve component", args.at(1).asString());
    auto emplace = compType.func("emplace"_hs);
    if (not compType) LOG::ERROR("Failed to resolve emplace function of", args.at(1).asString());

    ArgsVector compArgsVec(args.at(2).asTable());
    emplace.invoke({}, &reg, eid, &compArgsVec);

    return 0;
}

int EntityLuaApi::getComponent(Luna::Stack stack, Luna::TypeVector args)
{
    constexpr auto ARGS_ERROR =
        "get component requires 2 arguments, "
        "first should be entity table, "
        "second should be component name";
    GNG2D_ASSERT(args.size() == 2, ARGS_ERROR);
    GNG2D_ASSERT(args.at(0).isTable(), ARGS_ERROR);
    GNG2D_ASSERT(args.at(1).isString(), ARGS_ERROR);

    auto& enttable = args.at(0).asTable();
    auto  eid      = (entt::entity)enttable.get("entity"_hash).asInteger();
    GNG2D_ASSERT(reg.valid(eid), "Invalid entity in getComponent call");
    auto compHash      = args.at(1).asStringHash();
    auto componentType = entt::resolve(compHash);
    GNG2D_ASSERT(componentType);
    pushComponent(stack, eid, componentType);

    return 1;
}

int EntityLuaApi::hasComponent(Luna::Stack stack, Luna::TypeVector args)
{
    constexpr auto ARGS_ERROR =
        "has component requires 2 arguments, "
        "first should be entity table, "
        "second should be component name";
    GNG2D_ASSERT(args.size() == 2, ARGS_ERROR);
    GNG2D_ASSERT(args.at(0).isTable(), ARGS_ERROR);
    GNG2D_ASSERT(args.at(1).isString(), ARGS_ERROR);

    auto& enttable = args.at(0).asTable();
    auto  eid      = (entt::entity)enttable.get("entity"_hash).asInteger();
    GNG2D_ASSERT(reg.valid(eid), "Invalid entity in getComponent call");
    auto compHash      = args.at(1).asStringHash();
    auto componentType = entt::resolve(compHash);
    GNG2D_ASSERT(componentType);

    auto hasComponent = componentType.func("hasComponent"_hs);
    GNG2D_ASSERT(hasComponent);

    bool res = hasComponent.invoke({}, &reg, eid).cast<bool>();
    stack.pushBool(res);

    return 1;
}

int EntityLuaApi::component__index(Luna::Stack stack, Luna::TypeVector args)
{
    constexpr auto ARGS_ERROR =
        "component __index requires 2 arguments, "
        "first should be component handle, "
        "second should be component variable";
    GNG2D_ASSERT(args.size() == 2, ARGS_ERROR);
    GNG2D_ASSERT(args.at(0).isUserdata(), ARGS_ERROR);
    GNG2D_ASSERT(args.at(1).isString(), ARGS_ERROR);

    entt::meta_any component   = args.at(0).asUserdata().get<CompUserdata>().ref();
    auto           datumHash   = args.at(1).asStringHash();
    auto           datumHandle = component.get(datumHash);

    auto res = Gng2D::tryPushDatumOnStack(stack, component, datumHash);
    if (res.isSuccess()) return 1;

    LOG::ERROR("Failed __index call of", component.type().info().name(), "\n  -- ", res.asErr(),
               "for datum:", args.at(1).asString());
    return 0;
}

int EntityLuaApi::component__newindex(Luna::Stack stack, Luna::TypeVector args)
{
    constexpr auto ARGS_ERROR =
        "component __index requires 3 arguments, "
        "first should be component handle, "
        "second should be component variable, "
        "third should be new value of this variable";
    GNG2D_ASSERT(args.size() == 3, ARGS_ERROR);
    GNG2D_ASSERT(args.at(0).isUserdata(), ARGS_ERROR);
    GNG2D_ASSERT(args.at(1).isString(), ARGS_ERROR);

    auto component = args.at(0).asUserdata().get<CompUserdata>().ref();
    auto datumHash = args.at(1).asStringHash();

    auto res = Gng2D::trySetDatumFromLunaType(args.at(2), component, datumHash);
    if (res.isSuccess())
    {
        entt::entity e = args.at(0).asUserdata().get<CompUserdata>().owner;
        component.invoke("patchSignal"_hs, &reg, e);
        return 0;
    }

    LOG::ERROR("Failed __newindex call of", component.type().info().name(), "\n  --", res.asErr(),
               "for datum:", args.at(1).asString());

    return 0;
}
