#pragma once
#include "Gng2D/commons/luna/stack.hpp"
#include "Gng2D/commons/luna/state.hpp"
#include "Gng2D/commons/system_interface.hpp"

namespace Gng2D {
struct EntityLuaApi : SystemInterface
{
    EntityLuaApi(entt::registry& r, Luna::State&);
    EntityLuaApi(EntityLuaApi&)  = delete;
    EntityLuaApi(EntityLuaApi&&) = delete;
    ~EntityLuaApi();

    auto operator=(EntityLuaApi&)  = delete;
    auto operator=(EntityLuaApi&&) = delete;

    void onUpdate() override final;

    void setEntityTable(entt::entity, Luna::TableRef&);
    void pushComponent(Luna::Stack&, entt::entity, entt::meta_type);

  private:
    Luna::State&   lunaState;
    Luna::TableRef apiTable;

    int addComponent(Luna::Stack, Luna::TypeVector);
    int getComponent(Luna::Stack, Luna::TypeVector);
    int hasComponent(Luna::Stack, Luna::TypeVector);

    int component__newindex(Luna::Stack, Luna::TypeVector);
    int component__index(Luna::Stack, Luna::TypeVector);
};
}; // namespace Gng2D
