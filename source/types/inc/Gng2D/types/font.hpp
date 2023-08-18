#pragma once
#include <SDL2/SDL.h>

namespace Gng2D
{
struct Font 
{
    Font(SDL_Texture*, int charWidth, int charHeight, int charsPerRow);
    Font(const Font&)               = default;
    Font(Font&&)                    = default;
    Font& operator= (const Font&)   = default;
    Font& operator= (Font&&)        = default;

    void    renderChar(SDL_Renderer*, char, SDL_Rect dst, 
                uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) const;

    int     width()     const;
    int     height()    const;

private:
    int     charWidth;
    int     charHeight;
    int     charsPerRow;
    SDL_Texture*    fontTexture;
};
}

