#include "Gng2D/components/sprite.hpp"
#include "Gng2D/scene/assets.hpp"

using Gng2D::Sprite;

Sprite Sprite::loadFromAssets(entt::registry* reg, entt::hashed_string::hash_type id)
{
    return reg->ctx().get<Assets&>().getSprite(id);
}

entt::meta_factory<Sprite> Sprite::createMetaInfo()
{
    using namespace entt::literals;
    return entt::meta<Sprite>().type("Sprite"_hs).ctor<&Sprite::loadFromAssets>();
}
