#include "Gng2D/entities/lua_api.hpp"
#include "Gng2D/commons/args_vector.hpp"

using Gng2D::EntityLuaApi;
namespace Luna = Gng2D::Luna;
using LOG      = Gng2D::LOG;
using namespace entt::literals;

static int component__index(Luna::Stack stack, Luna::TypeVector args)
{
    constexpr auto ARGS_ERROR =
        "component __index requires 2 arguments, "
        "first should be component handle, "
        "second should be component variable";
    GNG2D_ASSERT(args.size() == 2, ARGS_ERROR);
    GNG2D_ASSERT(args.at(0).isUserdata(), ARGS_ERROR);
    GNG2D_ASSERT(args.at(1).isString(), ARGS_ERROR);

    entt::meta_any component = args.at(0).asUserdata().toMetaAny();
    auto           var       = component.get(args.at(1).asStringHash());
    if (not var) [[unlikely]]
    {
        LOG::ERROR("Failed to get", args.at(0).asString(), "from component",
                   component.type().info().name());
        return 0;
    }

    if (var.type().is_integral())
    {
        stack.pushInt(var.cast<Luna::Integer>());
        return 1;
    }
    else if (var.type().is_arithmetic() and var.allow_cast<Luna::Float>())
    {
        stack.pushFloat(var.cast<Luna::Float>());
        return 1;
    }
    else if (var.allow_cast<Luna::String>())
    {
        stack.pushString(var.cast<Luna::String>());
        return 1;
    }

    LOG::ERROR("Failed to convert component datum to lua type");
    return 0;
}

static int component__newindex(Luna::Stack stack, Luna::TypeVector args)
{
    constexpr auto ARGS_ERROR =
        "component __index requires 3 arguments, "
        "first should be component handle, "
        "second should be component variable, "
        "third should be new value of this variable";
    GNG2D_ASSERT(args.size() == 3, ARGS_ERROR);
    GNG2D_ASSERT(args.at(0).isUserdata(), ARGS_ERROR);
    GNG2D_ASSERT(args.at(1).isString(), ARGS_ERROR);

    auto component = args.at(0).asUserdata().toMetaAny();
    auto compDatum = args.at(1).asStringHash();

    auto datum = component.type().data(compDatum);
    if (datum.type().is_arithmetic())
    {
        if (args.at(2).isInteger())
        {
            if (datum.set(component, args.at(2).asInteger()))
                LOG::ERROR("Failed to set", args.at(1).asString(), "in",
                           component.type().info().name(), "with integer");
        }
        else if (args.at(2).isFloat())
        {
            if (datum.set(component, args.at(2).asFloat()))
                LOG::ERROR("Failed to set", args.at(1).asString(), "in",
                           component.type().info().name(), "with float");
        }
        else
        {
            LOG::ERROR("Cannot set datum", args.at(1).asString(), "of",
                       component.type().info().name());
        }
    }
    else if (datum.type().info() == entt::type_id<std::string>())
    {
        if (not datum.set(component, args.at(2).asString()))
            LOG::ERROR("Failed to set", args.at(1).asString(), "in",
                       component.type().info().name());
    }
    else
    {
        LOG::ERROR("Unhandled type at __newindex, for", args.at(1).asString(), "in",
                   component.type().info().name());
    }

    return 0;
}

EntityLuaApi::EntityLuaApi(entt::registry& r, Luna::State& ls)
    : SystemInterface(r)
    , lunaState(ls)
    , apiTable(lunaState.createTableRef())
{
    apiTable.createSubTable("componentMeta");
    auto componentMetaTable = apiTable.get("componentMeta").asTable();

    lunaState.registerFunction<component__index>("__index", componentMetaTable);
    lunaState.registerFunction<component__newindex>("__newindex", componentMetaTable);

    // REGISTER COMPONENTS
    lunaState.registerMethod<&EntityLuaApi::getComponent>(*this, "getComponent");
    lunaState.registerMethod<&EntityLuaApi::addComponent>(*this, "addComponent");
}

int EntityLuaApi::addComponent(Luna::Stack, Luna::TypeVector args)
{
    constexpr auto ARGS_ERROR =
        "add components requires 3 arguments, "
        "first should be entity id, "
        "second should be component name, "
        "third should be table with args";
    GNG2D_ASSERT(args.size() == 3, ARGS_ERROR);
    GNG2D_ASSERT(args.at(0).isInteger(), ARGS_ERROR);
    GNG2D_ASSERT(args.at(1).isString(), ARGS_ERROR);
    GNG2D_ASSERT(args.at(2).isTable(), ARGS_ERROR);

    auto  eid      = (entt::entity)args.at(0).asInteger();
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
        "get components requires 2 arguments, "
        "first should be entity id, "
        "second should be component name";
    GNG2D_ASSERT(args.size() == 2, ARGS_ERROR);
    GNG2D_ASSERT(args.at(0).isInteger(), ARGS_ERROR);
    GNG2D_ASSERT(args.at(1).isString(), ARGS_ERROR);

    auto eid = (entt::entity)args.at(0).asInteger();
    GNG2D_ASSERT(reg.valid(eid), "Invalid entity in getComponent call");
    auto compHash      = args.at(1).asStringHash();
    auto componentType = entt::resolve(compHash);
    GNG2D_ASSERT(componentType);

    auto getRef = componentType.func("getRef"_hs);
    GNG2D_ASSERT(getRef);

    auto component = stack.newUserdata<entt::meta_any>(getRef.invoke({}, &reg, eid));
    component.setMetaTable(apiTable.get("componentMeta").asTable());

    return 1;
}
