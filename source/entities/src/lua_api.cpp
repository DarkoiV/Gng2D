#include "Gng2D/entities/lua_api.hpp"
#include "Gng2D/components/meta/properties.hpp"

using Gng2D::EntityLuaApi;
namespace Luna = Gng2D::Luna;
using LOG      = Gng2D::LOG;
using Gng2D::FIELD_TYPE;
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

    entt::meta_any& component = *(entt::meta_any*)args.at(0).asUserdata().get();
    const auto*     metaInfo  = Gng2D::getMetaInfo(component.type());

    auto& data = metaInfo->data;
    if (not data)
    {
        LOG::WARN("Component", metaInfo->name, "has no data to assign from lua");
        return 0;
    }

    entt::meta_any datumHandle;
    FIELD_TYPE     datumType;
    for (const auto& datum: *data)
    {
        if (datum.name == args.at(1).asString())
        {
            datumHandle = component.get(datum.id);
            datumType   = datum.type;
            break;
        }
    }

    switch (datumType)
    {
    case FIELD_TYPE::FLOAT:
        GNG2D_ASSERT(datumHandle.allow_cast<float>());
        stack.push(datumHandle.cast<float>());
        return 1;
    case FIELD_TYPE::INTEGER:
        GNG2D_ASSERT(datumHandle.allow_cast<long>());
        stack.push(datumHandle.cast<long>());
        return 1;
    case FIELD_TYPE::STRING:
        GNG2D_ASSERT(datumHandle.allow_cast<std::string>());
        stack.push(datumHandle.cast<std::string>());
        return 1;
    }

    LOG::WARN("Failed to convert component datum to lua type");
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
    auto compHash      = entt::hashed_string::value(args.at(1).asString().c_str());
    auto componentType = entt::resolve(compHash);
    GNG2D_ASSERT(componentType);

    auto getRef = componentType.func("getRef"_hs);
    GNG2D_ASSERT(getRef);

    auto component = stack.newUserdata<entt::meta_any>(getRef.invoke({}, &reg, eid));
    component.setMetaTable(apiTable.get("componentMeta").asTable());

    return 1;
}
