#include "Gng2D/components/lua_script.hpp"
#include "Gng2D/core/global.hpp"
#include "util_macros.hpp"

using Gng2D::LuaScript;
using Datum = Gng2D::ComponentDatum;
using Arg   = Gng2D::ComponentArg;
using namespace entt::literals;

const inline static Gng2D::ComponentArgs LUA_SCRIPT_ARGS{
    Arg{.name          = "script",
        .description   = "script to be attached to entity",
        .acceptedTypes = {entt::type_id<std::string>()},
        .required      = true}
};

const inline static Gng2D::ComponentData LUA_SCRIPT_DATA{
    Datum{.id = "script"_hs, .name = "script", .type = Gng2D::FIELD_TYPE::STRING}
};

const inline static Gng2D::ComponentMetaInfo LUA_SCRIPT_META{.id   = "LuaScript"_hs,
                                                             .name = "LuaScript",
                                                             .args = LUA_SCRIPT_ARGS,
                                                             .data = LUA_SCRIPT_DATA};

std::optional<LuaScript> LuaScript::fromArgs(const ArgsVector&              args,
                                             const entt::registry::context& ctx)
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

    if (not scriptName.empty())
    {
        auto                  entityEnv = GLOBAL::LUNA_STATE.createTableRef();
        std::filesystem::path scriptPath =
            GLOBAL::DATA_DIRECTORY / "scripts" / (scriptName + ".lua");
        GLOBAL::LUNA_STATE.doFile(scriptPath, entityEnv);
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
