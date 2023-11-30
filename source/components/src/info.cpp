#include "Gng2D/components/info.hpp"
#include "Gng2D/commons/args_vector.hpp"
#include "util_macros.hpp"

using Gng2D::Info;
using namespace entt::literals;

std::optional<Info> Info::fromArgs(const ArgsVector& args, entt::registry::context&)
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

Info::MetaFactory Info::registerData(MetaFactory factory)
{
    return factory.data<&Info::name>("name"_hs);
}
