#include "Gng2D/gui/icon.hpp"
#include "Gng2D/components/sprite.hpp"
#include "Gng2D/core/asset_registry.hpp"

using Gng2D::Icon;

Icon::Icon(entt::registry& r, Position p, const std::string& spriteName)
    : Widget(r, p)
{
    setSprite(spriteName);
}

Icon::Icon(entt::registry& r, Widget& parent, RelativePosition rp, const std::string& spriteName)
    : Widget(r, parent, rp)
{
    setSprite(spriteName);
}

Gng2D::V2d Icon::getDimensions() const
{
    int x, y;
    auto* sprite = reg.get<Gng2D::Sprite>(root).texture;
    SDL_QueryTexture(sprite, nullptr, nullptr, &x, &y);

    return V2d{static_cast<float>(x), static_cast<float>(y)};
}

void Icon::setSprite(const std::string& spriteName)
{
    AssetRegistry assetRegistry;
    if (auto* sprite = assetRegistry.getSprite(spriteName))
    {
        SDL_Rect rect{};
        SDL_QueryTexture(sprite, nullptr, nullptr, &rect.w, &rect.h);
        reg.emplace<Sprite>(root, sprite, rect);
    }
}

