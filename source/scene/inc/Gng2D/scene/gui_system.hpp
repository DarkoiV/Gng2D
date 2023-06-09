#pragma once
#include <entt/entt.hpp>
#include "Gng2D/components/sprite.hpp"
#include "Gng2D/types/menu_builder.hpp"

namespace Gng2D
{
struct GuiSystem 
{
    GuiSystem(entt::registry&);
    GuiSystem(const GuiSystem&) = delete;
    GuiSystem(GuiSystem&&)      = delete;
    ~GuiSystem();

    template <typename GuiComponent>
    void connectGuiComponent(entt::registry& reg);

    template <typename GuiComponent>
    void disconnectGuiComponent(entt::registry& reg);

private:
    template <typename GuiComponent>
    static void attachGuiComponentSprite(entt::registry& reg, entt::entity e);

    template <typename GuiComponent>
    static void updateGuiComponentSprite(entt::registry& reg, entt::entity e);

    entt::registry& reg;
};


// TEMPLATE DEFINITIONS
template <typename GuiComponent>
void GuiSystem::connectGuiComponent(entt::registry& reg)
{
    constexpr bool ownsSprite = requires(GuiComponent& comp)
    {
        comp.getSprite();
    };
    static_assert(ownsSprite, "GuiComponent has to own sprite");
    reg.on_construct<GuiComponent>()
        .template connect<&attachGuiComponentSprite<GuiComponent>>();
    reg.on_update<GuiComponent>()
        .template connect<&updateGuiComponentSprite<GuiComponent>>();
}

template <typename GuiComponent>
void GuiSystem::disconnectGuiComponent(entt::registry& reg)
{
    reg.on_construct<GuiComponent>()
        .template disconnect<&attachGuiComponentSprite<GuiComponent>>();
    reg.on_update<GuiComponent>()
        .template disconnect<&updateGuiComponentSprite<GuiComponent>>();
}

template <typename GuiComponent>
void GuiSystem::attachGuiComponentSprite(entt::registry& reg, entt::entity e)
{
    auto& comp = reg.get<GuiComponent>(e);
    reg.emplace_or_replace<Gng2D::Sprite>(e, comp.getSprite());
}

template <typename GuiComponent>
void GuiSystem::updateGuiComponentSprite(entt::registry& reg, entt::entity e)
{
    auto& comp = reg.get<GuiComponent>(e);
    reg.replace<Gng2D::Sprite>(e, comp.getSprite());
}
}

