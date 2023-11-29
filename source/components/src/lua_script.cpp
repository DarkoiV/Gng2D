#include "Gng2D/components/lua_script.hpp"
#include "Gng2D/commons/luna/state.hpp"
#include "Gng2D/commons/repository.hpp"
#include "util_macros.hpp"

using Gng2D::LuaScript;
using Datum = Gng2D::ComponentDatum;
using Arg   = Gng2D::ComponentArg;
using namespace entt::literals;

const static Gng2D::ComponentArgs LUA_SCRIPT_ARGS{
    Arg{.name          = "script",
        .description   = "script to be attached to entity",
        .acceptedTypes = {entt::type_id<std::string>()},
        .required      = true}
};

const static Gng2D::ComponentData LUA_SCRIPT_DATA{
    Datum{.id = "script"_hs, .name = "script", .type = Gng2D::FIELD_TYPE::STRING}
};

const static Gng2D::ComponentMetaInfo LUA_SCRIPT_META{.id   = "LuaScript"_hs,
                                                      .name = "LuaScript",
                                                      .args = LUA_SCRIPT_ARGS,
                                                      .data = LUA_SCRIPT_DATA};

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

const Gng2D::ComponentMetaInfo* LuaScript::metaInfo()
{
    return &LUA_SCRIPT_META;
}

LuaScript::MetaFactory LuaScript::registerData(MetaFactory mf)
{
    return mf.data<&LuaScript::scriptName>("script"_hs);
}
