#pragma once
#include "Gng2D/components/meta/component_meta_info.hpp"
#include <entt/entt.hpp>
#include <vector>

namespace Gng2D {
struct Parent
{
    Parent(entt::entity p)
        : id_(p)
    {
    }

    entt::entity id() const { return id_; }

    static void onCreate(entt::registry&, entt::entity);
    static void onDelete(entt::registry&, entt::entity);

    static const ComponentMetaInfo* metaInfo();

  private:
    entt::entity id_;
};

struct Children
{
    using value_type = entt::entity;

    auto begin() const { return list.begin(); }
    auto end() const { return list.end(); }
    auto find(entt::entity e) const { return std::find(list.begin(), list.end(), e); }
    bool contains(entt::entity e) const { return find(e) != end(); }
    bool empty() { return list.empty(); }

    static void onDelete(entt::registry&, entt::entity);

    static const ComponentMetaInfo* metaInfo();

  private:
    friend Parent;
    std::vector<entt::entity> list;
    bool                      inProccessOfDeletion{false};
};
} // namespace Gng2D
