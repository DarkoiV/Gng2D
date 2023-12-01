#pragma once
#include "Gng2D/components/meta/properties_macro.hpp"
#include <entt/entt.hpp>
#include <vector>

namespace Gng2D {
struct Parent
{
    component_property_name(Parent);

    Parent(entt::entity p)
        : id_(p)
    {
    }

    entt::entity id() const { return id_; }

    static void onCreate(entt::registry&, entt::entity);
    static void onDelete(entt::registry&, entt::entity);

  private:
    entt::entity id_;
};

struct Children
{
    component_property_name(Children);

    using value_type = entt::entity;

    auto begin() const { return list.begin(); }
    auto end() const { return list.end(); }
    auto find(entt::entity e) const { return std::find(list.begin(), list.end(), e); }
    bool contains(entt::entity e) const { return find(e) != end(); }
    bool empty() const { return list.empty(); }
    auto size() const { return list.size(); }

    static void onDelete(entt::registry&, entt::entity);

  private:
    friend Parent;
    std::vector<entt::entity> list;
    bool                      inProccessOfDeletion{false};
};
} // namespace Gng2D
