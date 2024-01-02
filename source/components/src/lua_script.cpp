#include "Gng2D/components/lua_script.hpp"
#include "Gng2D/commons/args_vector.hpp"
#include "Gng2D/commons/luna/state.hpp"
#include "Gng2D/commons/repository.hpp" // IWYU pragma: keep
#include "Gng2D/entities/lua_api.hpp"
#include "Gng2D/scene/actions_handler.hpp"
#include "util_macros.hpp"

using Gng2D::LuaScript;
using namespace entt::literals;

void LuaScript::invokeAction(entt::registry& reg, HashedString action, bool release)
{
    auto stack    = reg.ctx().get<Luna::State>().getStack();
    auto callback = entityEnv.get("OnAction").asTable().get(action.data());
    stack.callFunction(callback.asFunction(), {release});
}

void LuaScript::onCreate(entt::registry& reg, entt::entity e)
{
    auto& luaScript = reg.get<LuaScript>(e);
    auto& env       = luaScript.entityEnv;
    reg.ctx().get<EntityLuaApi>().setEntityTable(e, env);

    if (auto OnUpdate = env.get("OnUpdate"); OnUpdate.isFunction())
    {
        luaScript.onUpdate = OnUpdate.asFunction();
    }

    if (auto OnAction = env.get("OnAction"); OnAction.isTable())
    {
        auto& aHandler = reg.ctx().get<ActionsHandler>();
        for (auto&& [action, callback]: OnAction.asTable())
        {
            GNG2D_ASSERT(action.isString() and callback.isFunction());
            auto sink       = aHandler.getActionSink(action.asHashedString());
            auto connection = sink.connect<&LuaScript::invokeAction>(luaScript);
            luaScript.connections.emplace_back(std::move(connection));
        }
    }

    if (auto OnHover = env.get("OnHover"); OnHover.isFunction())
    {
        luaScript.hasOnHover = true;
    }
}

void LuaScript::onSpawn(entt::registry& reg, entt::entity e)
{
    auto& env = reg.get<LuaScript>(e).entityEnv;
    if (auto onSpawn = env.get("OnSpawn"); onSpawn.isFunction())
    {
        auto stack = reg.ctx().get<Luna::State>().getStack();
        stack.callFunction(onSpawn.asFunction());
    }
}

void LuaScript::onDelete(entt::registry& reg, entt::entity e)
{
    auto& luaScript = reg.get<LuaScript>(e);
    for (auto& connection: luaScript.connections)
    {
        connection.release();
    }
}

std::optional<LuaScript> LuaScript::fromArgs(const ArgsVector& args, entt::registry::context& ctx)
{
    std::string scriptName;
    for (auto&& [id, arg]: args)
    {
        switch (id)
        {
            COMP_ARG_CASE("script", scriptName);
            UNKNOWN_ARG_CASE;
        }
    }

    if (auto pathOpt = Repository::getScriptPath(scriptName))
    {
        auto& luna      = ctx.get<Luna::State&>();
        auto  entityEnv = luna.createTableRef();
        luna.doFile(*pathOpt, entityEnv);
        return LuaScript(scriptName, entityEnv);
    }
    return std::nullopt;
}

void LuaScript::registerData(MetaFactory mf)
{
    mf.data<&LuaScript::scriptName>("script"_hs)
        .prop(ComponentFieldProperties::FIELD_TYPE, ComponentFieldType::STRING)
        .prop(ComponentFieldProperties::FIELD_NAME, "script");
}
