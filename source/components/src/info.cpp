#include "Gng2D/components/info.hpp"
#include "Gng2D/commons/args_vector.hpp"
#include "Gng2D/components/meta/properties.hpp"
#include "util_macros.hpp"

using namespace Gng2D;
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

void Info::registerData(MetaFactory factory)
{
    factory.data<&Info::name>("name"_hs).prop(ComponentFieldProperties::FIELD_TYPE,
                                              ComponentFieldType::STRING);
}
