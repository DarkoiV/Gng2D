#include "Gng2D/scene/component_library.hpp"
#include "Gng2D/components/sprite.hpp"
#include "Gng2D/components/transform2d.hpp"
#include "Gng2D/core/global_assets.hpp"

using Gng2D::ComponentLibrary;

ComponentLibrary::ComponentLibrary(entt::registry& r)
    : reg(r)
{
    entt::meta_reset();
    using namespace entt::literals;
    registerComponent<Gng2D::Sprite>("Sprite"_hs).ctor<&GlobalAssets::getSprite>();
    registerComponent<Gng2D::Transform2d>("Transform2d"_hs).ctor<float, float, int8_t, int8_t>();

    reg.ctx().emplace<ComponentLibrary&>(*this);
}

entt::meta_type ComponentLibrary::getMeta(entt::hashed_string name)
{
    return entt::resolve(name);
}
