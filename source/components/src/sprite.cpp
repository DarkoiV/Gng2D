#include "Gng2D/components/sprite.hpp"
#include "Gng2D/scene/assets.hpp"
#include "Gng2D/commons/log.hpp"

using Gng2D::Sprite;

Sprite Sprite::loadFromAssets(entt::registry* reg, entt::hashed_string::hash_type id)
{
    auto optSprite = reg->ctx().get<Assets&>().getSprite(id);
    if (optSprite) [[likely]] return *optSprite;

    LOG::WARN("Failed to load sprite with id:", id);
    return Sprite{};
}

entt::meta_factory<Sprite> Sprite::createMetaInfo()
{
    using namespace entt::literals;
    return entt::meta<Sprite>()
        .type("Sprite"_hs)
        .ctor<&Sprite::loadFromAssets>();
}

