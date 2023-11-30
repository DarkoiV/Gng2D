#include "Gng2D/components/lua_script.hpp"
#include "Gng2D/commons/args_vector.hpp"
#include "Gng2D/commons/luna/state.hpp"
#include "Gng2D/commons/repository.hpp" // IWYU pragma: keep
#include "util_macros.hpp"

using Gng2D::LuaScript;
using namespace entt::literals;

void LuaScript::OnCreate(entt::registry& reg, entt::entity e)
{
    auto& state = reg.ctx().get<Luna::State&>();
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

    if (auto pathOpt = Repository::getScript(scriptName))
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
        .prop(ComponentFieldProperties::FIELD_TYPE, ComponentFieldType::STRING);
}
