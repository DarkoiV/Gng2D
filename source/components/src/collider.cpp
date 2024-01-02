#include "Gng2D/components/collider.hpp"
#include "Gng2D/components/meta/properties.hpp"
#include "util_macros.hpp"

using namespace Gng2D;
using namespace entt::literals;

void Collider::onCreate(entt::registry& reg, entt::entity e)
{
    auto& collider = reg.get<Collider>(e);
    switch (collider.type)
    {
    case Collider::Type::BOX:
        auto&& groupStorage = reg.storage<detail::BoxCollider>(collider.group);
        groupStorage.emplace(e, collider.dimension1, collider.dimension2);
        return;
    }
}

void Collider::onDelete(entt::registry& reg, entt::entity e)
{
    auto& collider = reg.get<Collider>(e);
    switch (collider.type)
    {
    case Collider::Type::BOX:
        auto&& groupStorage = reg.storage<detail::BoxCollider>(collider.group);
        groupStorage.remove(e);
        return;
    }
}

std::optional<Collider> Collider::fromArgs(const ArgsVector& args, entt::registry::context&)
{
    Collider    collider;
    std::string type;
    std::string group;

    for (auto&& [id, arg]: args)
    {
        switch (id)
        {
            COMP_ARG_CASE("group", group);
            COMP_ARG_CASE("type", type);
            COMP_ARG_CASE("width", collider.dimension1);
            COMP_ARG_CASE("height", collider.dimension2);
            UNKNOWN_ARG_CASE;
        }
    }

    collider.group = entt::hashed_string(group.c_str());
    collider.type  = static_cast<Collider::Type>(entt::hashed_string::value(type.c_str()));

    switch (collider.type)
    {
    case Collider::Type::BOX:
        return collider;
    }

errReturn:
    LOG::ERROR("Failed to construct collider");
    return std::nullopt;
}
