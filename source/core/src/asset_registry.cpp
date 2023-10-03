#include "SDL2/SDL_image.h"
#include "SDL2/SDL.h"
#include "Gng2D/core/asset_registry.hpp"
#include "Gng2D/core/log.hpp"
#include "Gng2D/core/window.hpp"

using Gng2D::AssetRegistry;

void AssetRegistry::loadSprite(const std::string& name)
{
    if (globalSprites.contains(name)) return LOG::WARN("Sprite", name, "already loaded");
    if (auto* texture = loadTextureFile(name))
    {
        SDL_Rect textureRect{0, 0, 0, 0};
        SDL_QueryTexture(texture, nullptr, nullptr, &textureRect.w, &textureRect.h);
        globalSprites[name] = Gng2D::Sprite(texture, textureRect); 
    }
}

void AssetRegistry::loadBMFont(const std::string& name, int charW, int charH)
{
    if (globalFonts.contains(name)) return LOG::WARN("Font", name, "already loaded");
    auto* fontTexture = loadTextureFile(name, "data/fonts/");
    if (not fontTexture) return;

    int width;
    SDL_QueryTexture(fontTexture, nullptr, nullptr, &width, nullptr);
    int charsPerRow = width / charW;

    globalFonts.emplace(name, Font(fontTexture, charW, charH, charsPerRow));
    SDL_Rect textureRect{0, 0, 0, 0};
    SDL_QueryTexture(fontTexture, nullptr, nullptr, &textureRect.w, &textureRect.h);
    globalSprites["fonts/" + name] = Gng2D::Sprite(fontTexture, textureRect);
}

SDL_Texture* AssetRegistry::loadTextureFile(const std::string& name, const std::string& path)
{
    LOG::INFO("Loading:", name);
    const std::string pathToSprite{path + name + ".png"};
    auto sprite = IMG_LoadTexture(renderer, pathToSprite.c_str());

    if (not sprite) LOG::ERROR("Failed to load:", name, "SDL Error:", SDL_GetError());
    return sprite;
}

std::optional<Gng2D::Sprite> AssetRegistry::getSprite(const std::string& name) const
{
    if (not globalSprites.contains(name))
    {
        LOG::ERROR("No sprite:", name);
        return std::nullopt;
    }
    return globalSprites.at(name);
}

Gng2D::Font AssetRegistry::getFont(const std::string& name) const
{
    if (not globalFonts.contains(name))
    {
        LOG::ERROR("No font:", name);
        return Font(nullptr, 0, 0, 0);
    }
    return globalFonts.at(name);
}


void Gng2D::AssetRegistry::freeAllSprites()
{
    LOG::INFO("Freeing loaded sprites");
    for (const auto& [_, sprite] : globalSprites)
    {
        SDL_DestroyTexture(sprite.texture);
    }
}

AssetRegistry::RenderToTexture::RenderToTexture(int width, int height, Commands commands)
{
    if (target) SDL_DestroyTexture(target);
    target = SDL_CreateTexture(renderer, 
                               SDL_PIXELFORMAT_ARGB32,
                               SDL_TEXTUREACCESS_TARGET, 
                               width, 
                               height);

    auto err = SDL_SetRenderTarget(renderer, target);
    if (err) LOG::ERROR("Cannot change render target:", SDL_GetError());
 
    SDL_SetTextureBlendMode(target, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0,0,0,0); 
    SDL_RenderClear(renderer);

    commands(renderer);
}

AssetRegistry::RenderToTexture::~RenderToTexture()
{
    SDL_SetRenderTarget(renderer, NULL);
    if (not transferredOwnership) SDL_DestroyTexture(target);
}

SDL_Texture* AssetRegistry::RenderToTexture::getTexture()
{
    transferredOwnership = true;
    return target;
}

void AssetRegistry::RenderToTexture::saveTextureAsSprite(const std::string& name)
{
    if (globalSprites.contains(name)) return LOG::WARN("Sprite", name, "already loaded");

    transferredOwnership = true;
    SDL_Rect textureRect{0, 0, 0, 0};
    SDL_QueryTexture(target, nullptr, nullptr, &textureRect.w, &textureRect.h);
    globalSprites[name] = Gng2D::Sprite(target, textureRect);
}

