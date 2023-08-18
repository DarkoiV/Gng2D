#include "Gng2D/core/log.hpp"
#include "Gng2D/types/font.hpp"

using namespace Gng2D;

Font::Font(SDL_Texture* t, int cw, int ch, int cpr)
    : charWidth(cw)
    , charHeight(ch)
    , charsPerRow(cpr)
    , fontTexture(t)
{
    LOG::INFO("Loaded font dimensions", charWidth, charHeight, charsPerRow);
}

void Font::renderChar(SDL_Renderer* r, char c, SDL_Rect dst,
                      uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) const
{
    SDL_SetTextureAlphaMod(fontTexture, alpha);
    SDL_SetTextureColorMod(fontTexture, red, green, blue);

    const char offsetedChar = c - 32;
    int srcx = offsetedChar % charsPerRow * charWidth;
    int srcy = offsetedChar / charsPerRow * charHeight;

    SDL_Rect src = {srcx, srcy, charWidth, charHeight};
    SDL_RenderCopy(r, fontTexture, &src, &dst);
}

int Font::width() const
{
    return charWidth;
}

int Font::height() const
{
    return charHeight;
}


