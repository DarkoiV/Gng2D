#pragma once
#include "Gng2D/commons/args_vector.hpp"
#include "Gng2D/commons/types.hpp"
#include "Gng2D/components/component_args.hpp"
#include <entt/entt.hpp>

namespace Gng2D::Component {
enum PROPERTIES : StringHash {
    NAME      = entt::hashed_string::value("name"),
    ID        = entt::hashed_string::value("id"),
    ARGS_INFO = entt::hashed_string::value("args"),
};

template <typename C>
concept Default = requires(const C c, const Gng2D::ArgsVector& av) {
    {
        C::fromArgs(av)
    } -> std::same_as<C>;
    {
        C::argsInfo()
    } -> std::same_as<const Gng2D::ComponentArgs*>;
};

template <Default Comp>
const std::string& getName()
{
    return *((std::string*)entt::resolve<Comp>().prop(PROPERTIES::NAME).value().data());
}

template <Default Comp>
const StringHash getId()
{
    return *((StringHash*)entt::resolve<Comp>().prop(PROPERTIES::ID).value().data());
}

template <Default Comp>
const ComponentArgs* getArgsInfo()
{
    return entt::resolve<Comp>().prop(PROPERTIES::ARGS_INFO).value().template cast<const ComponentArgs*>();
}

} // namespace Gng2D::Component
