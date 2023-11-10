#include "Gng2D/core/global_assets.hpp"
#include "Gng2D/commons/log.hpp"
#include "Gng2D/core/global.hpp"
#include <SDL2/SDL_image.h>

using Gng2D::GlobalAssets;

void GlobalAssets::loadGlobalSprite(const std::string& name,
                                    const std::string& path,
                                    const std::string& fileType)
{
    const auto fullPath = GLOBAL::DATA_DIRECTORY + path + name + fileType;
    if (sprites.contains(entt::hashed_string{name.c_str()}.value()))
    {
        LOG::WARN("Sprite with hash coresponding to name:", name,
                  "already loaded. Collision possible");
        return;
    }

    Gng2D::Sprite loadedSprite;
    loadedSprite.texture = IMG_LoadTexture(GLOBAL::RENDERER, fullPath.c_str());
    if (not loadedSprite.texture) return LOG::ERROR("Failed to load", name, IMG_GetError());
    SDL_QueryTexture(loadedSprite.texture, nullptr, nullptr, &loadedSprite.srcRect.w,
                     &loadedSprite.srcRect.h);
    sprites[entt::hashed_string{name.c_str()}] = loadedSprite;

    LOG::OK("Loaded sprite:", name);
}

Gng2D::Sprite GlobalAssets::getSprite(const entt::hashed_string::hash_type id)
{
    auto it = sprites.find(id);
    if (it != sprites.end())
    {
        return it->second;
    }

    LOG::WARN("Sprite not found:", id);
    return Gng2D::Sprite{};
}

void GlobalAssets::deleteAllSprites()
{
    LOG::INFO("Deleting global assets");
    for (auto& [_, sprite]: sprites)
    {
        SDL_DestroyTexture(sprite.texture);
    }
}
