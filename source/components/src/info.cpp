#include "Gng2D/components/info.hpp"
#include "Gng2D/commons/args_vector.hpp"
#include "Gng2D/components/meta/component_userdata.hpp"
#include "Gng2D/components/meta/properties.hpp"
#include "util_macros.hpp"

using namespace Gng2D;
using namespace entt::literals;

int Info::lua__index(Luna::Stack stack, Luna::TypeVector args)
{
    constexpr auto ARGS_ERROR =
        "component __index requires 2 arguments, "
        "first should be component handle, "
        "second should be component variable";
    GNG2D_ASSERT(args.size() == 2, ARGS_ERROR);
    GNG2D_ASSERT(args.at(0).isUserdata(), ARGS_ERROR);
    GNG2D_ASSERT(args.at(1).isString(), ARGS_ERROR);

    ComponentUserdata& component = args.at(0).asUserdata().get<ComponentUserdata>();
    if (args.at(1).asString() == "name")
    {
        stack.pushString(((Info*)component.ptr)->name);
        return 1;
    }

    return 0;
}

int Info::lua__newindex(Luna::Stack stack, Luna::TypeVector args)
{
    constexpr auto ARGS_ERROR =
        "component __index requires 3 arguments, "
        "first should be component handle, "
        "second should be component variable, "
        "third should be new value of this variable";
    GNG2D_ASSERT(args.size() == 3, ARGS_ERROR);
    GNG2D_ASSERT(args.at(0).isUserdata(), ARGS_ERROR);
    GNG2D_ASSERT(args.at(1).isString(), ARGS_ERROR);

    ComponentUserdata& component = args.at(0).asUserdata().get<ComponentUserdata>();
    if (args.at(1).asString() == "name")
    {
        if (args.at(2).isString())
        {
            ((Info*)component.ptr)->name = args.at(2).asString();
        }
        component.owner.patch<Info>();
        return 0;
    }
    return 0;
}

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
    factory.data<&Info::name>("name"_hs)
        .prop(ComponentFieldProperties::FIELD_TYPE, ComponentFieldType::STRING)
        .prop(ComponentFieldProperties::FIELD_NAME, "name");
}
