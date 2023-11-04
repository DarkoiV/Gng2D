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

void ComponentLibrary::init()
{
    using namespace entt::literals;
    registerComponent<Gng2D::Sprite>();
    registerComponent<Gng2D::TransformPosition>();
    initialized = true;
}

