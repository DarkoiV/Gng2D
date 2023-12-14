#include "Gng2D/entities/lua_api.hpp"
#include "Gng2D/commons/args_vector.hpp"
#include "Gng2D/components/lua_script.hpp"
#include "Gng2D/components/meta/component_userdata.hpp"
#include "Gng2D/components/meta/properties.hpp"
#include "Gng2D/components/transform.hpp"

using Gng2D::EntityLuaApi;
using LOG = Gng2D::LOG;
using namespace entt::literals;
using CompUserdata = Gng2D::ComponentUserdata;

EntityLuaApi::EntityLuaApi(entt::registry& r, Luna::State& ls)
    : SystemInterface(r)
    , lunaState(ls)
    , apiTable(lunaState.createTableRef())
{

    for (auto&& [id, type]: entt::resolve())
    {
        if (not type.prop("exposesLuaApi"_hash)) continue;

        auto createMetaTable = type.func("createMetaTable"_hash);
        GNG2D_ASSERT(createMetaTable);
        apiTable.createSubTable(type.id());
        auto compMetaTable = apiTable.get(type.id()).asTable();
        createMetaTable.invoke({}, &lunaState, &compMetaTable);
        LOG::INFO("Created meta table for", type.info().name());
    }

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
        if (script.onUpdate)
        {
            lunaState.getStack().callFunction(*script.onUpdate);
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
    lunaState.registerMethod<&EntityLuaApi::getPosition>(*this, "getPosition", env);
}

void EntityLuaApi::pushComponent(Luna::Stack& stack, entt::entity e, entt::meta_type type)
{
    auto getRef = type.func("getRef"_hs);
    GNG2D_ASSERT(getRef);

    auto* componentPtr  = getRef.invoke({}, &reg, e).data();
    auto  ownerHandle   = entt::handle(reg, e);
    auto  component     = stack.newUserdata<ComponentUserdata>(ownerHandle, componentPtr);
    auto  componentMeta = apiTable.get(type.id());
    GNG2D_ASSERT(componentMeta.isTable());
    component.setMetaTable(componentMeta.asTable());
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

int EntityLuaApi::getPosition(Luna::Stack stack, Luna::TypeVector args)
{
    constexpr auto ARGS_ERROR =
        "has component requires 1 argument, "
        "which is entity table";
    GNG2D_ASSERT(args.size() == 1, ARGS_ERROR);
    GNG2D_ASSERT(args.at(0).isTable(), ARGS_ERROR);

    auto eid = (entt::entity)args.at(0).asTable().get("entity"_hash).asInteger();
    GNG2D_ASSERT(reg.all_of<detail::Position>(eid));
    auto& pos = reg.get<detail::Position>(eid);

    stack.pushFloat(pos.x);
    stack.pushFloat(pos.y);

    return 2;
}
