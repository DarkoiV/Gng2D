#include "Gng2D/scene/component_library.hpp"
#include "Gng2D/commons/log.hpp"
#include "Gng2D/components/input.hpp"
#include "Gng2D/components/layer.hpp"
#include "Gng2D/components/position.hpp"
#include "Gng2D/components/sprite.hpp"
#include "Gng2D/scene/assets.hpp"

using Gng2D::ComponentLibrary;
using Gng2D::LOG;

ComponentLibrary::ComponentLibrary(entt::registry& r)
    : reg(r)
{
    if (not initialized) init();
}

entt::meta_type ComponentLibrary::getMeta(entt::hashed_string name)
{
    return entt::resolve(name);
}

static Gng2D::Sprite loadFromAssets(entt::registry* reg, entt::hashed_string name)
{
    LOG::TRACE("Loading constructor for sprite");
    auto& assets = reg->ctx().get<Gng2D::Assets&>();
    if (auto sprite = assets.getSprite(name)) return *sprite;

    LOG::DEBUG("No sprite:", name.data());
    return Gng2D::Sprite{};
}

void ComponentLibrary::init()
{
    using namespace entt::literals;
    registerComponent<Gng2D::Sprite>("Sprite"_hs)
        .ctor<&loadFromAssets>();
    registerComponent<Gng2D::TransformPosition>("TransformPosition"_hs);
    registerComponent<Gng2D::TransformLayer>("TransformLayer"_hs);
    registerComponent<Gng2D::ActionListener>("ActionListener"_hs);
    initialized = true;
}

