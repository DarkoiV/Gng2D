#include "Gng2D/entities/lua_api.hpp"
#include "Gng2D/commons/args_vector.hpp"
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

    entt::meta_any component   = args.at(0).asUserdata().toMetaAny();
    auto           datumHash   = args.at(1).asStringHash();
    auto           datumHandle = component.get(datumHash);
    if (not datumHandle) [[unlikely]]
    {
        LOG::ERROR("Failed to get", args.at(0).asString(), "from component",
                   component.type().info().name());
        return 0;
    }

    auto field = component.type()
                     .data(datumHash)
                     .prop(Gng2D::ComponentFieldProperties::FIELD_TYPE)
                     .value()
                     .cast<Gng2D::ComponentFieldType>();

    switch (field)
    {
    case Gng2D::INTEGER:
    case Gng2D::STRING_HASH:
        if (not datumHandle.allow_cast<Luna::Integer>()) break;
        stack.pushInt(datumHandle.cast<Luna::Integer>());
        return 1;
    case Gng2D::FLOAT:
        if (not datumHandle.allow_cast<Luna::Float>()) break;
        stack.pushFloat(datumHandle.cast<Luna::Float>());
        return 1;
    case Gng2D::STRING:
        if (not datumHandle.allow_cast<Luna::String>()) break;
        stack.pushString(datumHandle.cast<Luna::String>());
        return 1;
    [[unlikely]] case Gng2D::UNDEF:
        break;
    }

    LOG::ERROR("Failed to convert component datum to lua type in __index call");
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
    auto datumHash = args.at(1).asStringHash();

    auto datumType = component.type().data(datumHash);
    auto field     = component.type()
                     .data(datumHash)
                     .prop(Gng2D::ComponentFieldProperties::FIELD_TYPE)
                     .value()
                     .cast<Gng2D::ComponentFieldType>();
    switch (field)
    {
    case Gng2D::STRING_HASH:
        if (args.at(2).isString())
        {
            if (datumType.set(component, args.at(2).asStringHash())) [[unlikely]]
            {
                LOG::ERROR("Failed to set", args.at(1).asString(), "in",
                           component.type().info().name(), "with integer");
            }
            return 0;
        }
        goto INTEGER_CASE_LABEL;
    case Gng2D::FLOAT:
        if (args.at(2).isFloat())
        {
            if (datumType.set(component, args.at(2).asFloat())) [[unlikely]]
            {
                LOG::ERROR("Failed to set", args.at(1).asString(), "in",
                           component.type().info().name(), "with float");
            }
            return 0;
        }
    /* NO BREAK! */
    case Gng2D::INTEGER:
    INTEGER_CASE_LABEL:
        if (args.at(2).isInteger())
        {
            if (datumType.set(component, args.at(2).asInteger())) [[unlikely]]
            {
                LOG::ERROR("Failed to set", args.at(1).asString(), "in",
                           component.type().info().name(), "with integer");
            }
            return 0;
        }
        break;
    case Gng2D::STRING:
        if (args.at(2).isString())
        {
            if (not datumType.set(component, args.at(2).asString())) [[unlikely]]
            {
                LOG::ERROR("Failed to set", args.at(1).asString(), "in",
                           component.type().info().name());
            }
            return 0;
        }
        break;
    [[unlikely]] case Gng2D::UNDEF:
        break;
    }
    LOG::ERROR("Unhandled type at __newindex, for", args.at(1).asString(), "in",
               component.type().info().name());
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
