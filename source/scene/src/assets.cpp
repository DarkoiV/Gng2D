#include "Gng2D/scene/assets.hpp"
#include "Gng2D/commons/log.hpp"
#include "Gng2D/core/global.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

using Gng2D::Assets;

void Assets::loadGlobalSprite(const std::string& name, const std::string& path, const std::string& fileType)
{
    const auto fullPath = GLOBAL::DATA_DIRECTORY + path + name + fileType;
    if (globalSprites.contains(entt::hashed_string{name.c_str()}.value())) 
    {
        LOG::WARN("Sprite with hash coresponding to name:", name, "already loaded. Collision possible");
        return;
    }

    Gng2D::Sprite loadedSprite;
    loadedSprite.texture = IMG_LoadTexture(GLOBAL::RENDERER, fullPath.c_str());
    if (not loadedSprite.texture) return LOG::ERROR("Failed to load", name, IMG_GetError());
    SDL_QueryTexture(loadedSprite.texture, 
            nullptr, 
            nullptr, 
            &loadedSprite.srcRect.w, 
            &loadedSprite.srcRect.h);
    globalSprites[entt::hashed_string{name.c_str()}] = loadedSprite;

    LOG::OK("Loaded sprite:", name);
}

std::optional<Gng2D::Sprite> Assets::getSprite(const entt::hashed_string::value_type id)
{
    auto it = globalSprites.find(id);
    if (it != globalSprites.end())
    {
        return it->second;
    }

    LOG::WARN("Sprite not found:", id);
    return std::nullopt;
}

