#include "Gng2D/entities/lua_api.hpp"
#include "Gng2D/components/meta/properties.hpp"

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

EntityLuaApi::EntityLuaApi(entt::registry& r, Luna::State& ls)
    : SystemInterface(r)
    , lunaState(ls)
    , apiTable(lunaState.createTableRef())
{
    apiTable.createSubTable("componentMeta");
    auto componentMetaTable = apiTable.get("componentMeta").asTable();

    lunaState.registerFunction<component__index>("__index", componentMetaTable);

    // REGISTER COMPONENTS
    lunaState.registerMethod<&EntityLuaApi::getComponent>(*this, "getComponent");
}

int EntityLuaApi::addComponent(Luna::Stack, Luna::TypeVector args)
{
    constexpr auto ARGS_ERROR =
        "add components requires 2 arguments, "
        "first should be entity id, "
        "second should be component name";
    GNG2D_ASSERT(args.size() == 2, ARGS_ERROR);
    GNG2D_ASSERT(args.at(0).isInteger(), ARGS_ERROR);
    GNG2D_ASSERT(args.at(1).isString(), ARGS_ERROR);

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
