#include "Gng2D/components/info.hpp"
#include "util_macros.hpp"

using Gng2D::Info;
using Datum = Gng2D::ComponentDatum;
using Arg   = Gng2D::ComponentArg;
using namespace entt::literals;

const static Gng2D::ComponentArgs INFO_ARGS{
    Arg{.name          = "name",
        .description   = "name for the given entity",
        .acceptedTypes = {entt::type_id<std::string>()},
        .required      = true}
};

const static Gng2D::ComponentData INFO_DATA{
    Datum{.id = "name"_hs, .name = "name", .type = Gng2D::FIELD_TYPE::STRING}
};

const static Gng2D::ComponentMetaInfo INFO_META{.id   = entt::hashed_string::value("Info"),
                                                .name = "Info",
                                                .args = INFO_ARGS,
                                                .data = INFO_DATA};

std::optional<Info> Info::fromArgs(const ArgsVector& args, const entt::registry::context&)
{
    Info info;
    for (auto&& [id, arg]: args)
    {
        switch (id)
        {
            COMP_ARG_CASE("name", info.name);
            UNKNOWN_ARG_CASE;
        }
    }

    if (info.name.empty()) return std::nullopt;
    else return info;
}

const Gng2D::ComponentMetaInfo* Info::metaInfo()
{
    return &INFO_META;
}

Info::MetaFactory Info::registerData(MetaFactory factory)
{
    return factory.data<&Info::name>("name"_hs);
}
