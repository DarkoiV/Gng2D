#pragma once
#include <map>
#include <memory>
#include <string>
#include <functional>
#include <optional>
#include <SDL2/SDL.h>
#include "Gng2D/types/font.hpp"
#include "Gng2D/components/sprite.hpp"

struct SDL_Texture;

namespace Gng2D
{
struct Application;
struct AssetRegistry
{
    void    loadSprite(const std::string& name);
    void    loadBMFont(const std::string& name, int charW, int charH);
    std::optional<Sprite>   getSprite(const std::string& name)  const;
    Font                    getFont(const std::string& name)    const;

    struct RenderToTexture
    {
        using Commands = std::function<void(SDL_Renderer*)>;
        RenderToTexture(int width, int height, Commands);
        ~RenderToTexture();

        SDL_Texture*    getTexture();
        void            saveTextureAsSprite(const std::string& name);

    private:
        bool            transferredOwnership{false};
        SDL_Texture*    target{nullptr};
    };

private:
    SDL_Texture*    loadTextureFile(const std::string& name, 
                                   const std::string& path = "data/sprites/");

    inline static std::map<std::string, Sprite> globalSprites;
    inline static std::map<std::string, Font>   globalFonts;

    /// ONLY FOR APPLICATION ///
    friend struct   Application;
    void            freeAllSprites();
    inline static   SDL_Renderer* renderer;
};
}

