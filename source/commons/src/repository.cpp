#include "Gng2D/commons/repository.hpp"
#include "Gng2D/commons/log.hpp"
#include "Gng2D/core/global.hpp"
#include <SDL2/SDL_image.h>

using Gng2D::Repository;

void Repository::loadSprite(const std::string& name,
                            const std::string& path,
                            const std::string& fileType)
{
    const auto fullPath       = GLOBAL::DATA_DIRECTORY + path + name + fileType;
    const auto spriteNameHash = entt::hashed_string{name.c_str()};
    if (sprites.contains(spriteNameHash))
    {
        if (spriteNames[spriteNameHash] == name) return LOG::WARN("Sprite already loaded");
        else
        {
            LOG::ERROR("Sprite collision", name, " has the same hash as ",
                       spriteNames[spriteNameHash]);
        }
        return;
    }

    Gng2D::Sprite loadedSprite;
    loadedSprite.texture = IMG_LoadTexture(GLOBAL::RENDERER, fullPath.c_str());
    if (not loadedSprite.texture) return LOG::ERROR("Failed to load", name, IMG_GetError());
    SDL_QueryTexture(loadedSprite.texture, nullptr, nullptr, &loadedSprite.srcRect.w,
                     &loadedSprite.srcRect.h);
    sprites[spriteNameHash] = loadedSprite;

    LOG::OK("Loaded sprite:", name);
}

Gng2D::Sprite Repository::getSprite(const entt::hashed_string::hash_type id)
{
    auto it = sprites.find(id);
    if (it != sprites.end())
    {
        return it->second;
    }

    LOG::ERROR("Sprite not found:", id);
    return Gng2D::Sprite{};
}

void Repository::freeResources()
{
    LOG::INFO("Deleting global assets");
    for (auto& [_, sprite]: sprites)
    {
        SDL_DestroyTexture(sprite.texture);
    }
    sprites.clear();
    spriteNames.clear();
}
