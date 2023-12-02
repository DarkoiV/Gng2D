#include "Gng2D/components/lua_script.hpp"
#include "Gng2D/commons/args_vector.hpp"
#include "Gng2D/commons/luna/state.hpp"
#include "Gng2D/commons/repository.hpp" // IWYU pragma: keep
#include "util_macros.hpp"

using Gng2D::LuaScript;
using namespace entt::literals;

void LuaScript::onSpawn(entt::registry& reg, entt::entity e)
{
    auto& env = reg.get<LuaScript>(e).entityEnv;
    if (auto onSpawn = env.get("OnSpawn"); onSpawn.isFunction())
    {
        auto stack = reg.ctx().get<Luna::State>().getStack();
        stack.callFunction(onSpawn.asFunction());
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
